#include "RenderSystem.h"
#include "../Debug/Debug.h"
#include "../Debug/Timer.h"

#include "Ecs/Scene.h"

#include "Core/Application.h"

namespace Seidon
{
	static double time = 0;
	RenderSystem::RenderSystem()
		: uiRenderer(300, 100, 10, 10000)
	{
		depthShader = new Shader();
		skinnedDepthShader = new Shader();
		cubemapShader = new Shader();
		quadShader = new Shader();
	}

	void RenderSystem::Init()
	{
		framebufferWidth = window->GetWidth();
		framebufferHeight = window->GetHeight();

		captureCube.Init();
		fullscreenQuad.Init();

		hdrMap.Create(window->GetWidth(), window->GetHeight(), (unsigned char*)NULL, TextureFormat::RGBA, TextureFormat::FLOAT16_ALPHA);
		entityMap.Create(window->GetWidth(), window->GetHeight(), (int*)NULL, TextureFormat::RED_INTEGER, TextureFormat::INT32);
		hdrDepthStencilBuffer.Create(window->GetWidth(), window->GetHeight(), RenderBufferType::DEPTH_STENCIL);

		hdrFramebuffer.Create();
		hdrFramebuffer.SetColorTexture(hdrMap);
		hdrFramebuffer.SetColorTexture(entityMap, 1);
		hdrFramebuffer.SetDepthStencilRenderBuffer(hdrDepthStencilBuffer);

		int shadowSamplers[CASCADE_COUNT];
		for (int i = 0; i < CASCADE_COUNT; i++)
		{
			shadowMaps[i].Create(SHADOW_MAP_SIZE, SHADOW_MAP_SIZE, (unsigned char*)NULL, TextureFormat::DEPTH, TextureFormat::DEPTH, ClampingMode::BORDER, glm::vec3(1), false);
			depthFramebuffers[i].Create();
			depthFramebuffers[i].SetDepthTexture(shadowMaps[i]);
			depthFramebuffers[i].DisableColorBuffer();
			shadowSamplers[i] = 8 + i;
		}

		renderTarget.Create(window->GetWidth(), window->GetHeight(), (unsigned char*)NULL, TextureFormat::RGBA, TextureFormat::RGBA);
		renderDepthStencilBuffer.Create(window->GetWidth(), window->GetHeight(), RenderBufferType::DEPTH_STENCIL);
		renderFramebuffer.Create();
		renderFramebuffer.SetColorTexture(renderTarget);
		renderFramebuffer.SetDepthStencilRenderBuffer(renderDepthStencilBuffer);

		depthShader->Load("Shaders/ShadowPass.shader");
		skinnedDepthShader->Load("Shaders/ShadowPass-Skinned.sdshader");
		quadShader->Load("Shaders/Simple.shader");
		cubemapShader->Load("Shaders/Cubemap.shader");

		GL_CHECK(glEnable(GL_DEPTH_TEST));
		//GL_CHECK(glEnable(GL_BLEND));
		glDisable(GL_DITHER);
		GL_CHECK(glEnable(GL_CULL_FACE));
		GL_CHECK(glEnable(GL_TEXTURE_CUBE_MAP_SEAMLESS));

		windowResizeCallbackPosition = 
			window->AddWindowSizeCallback([this](int width, int height)
			{
				if (useFullWindow && width > 0 && height > 0)
					ResizeFramebuffer(width, height);
			});

		renderFramebuffer.Unbind();

		GL_CHECK(glGenBuffers(2, pbos));

		GL_CHECK(glBindBuffer(GL_PIXEL_PACK_BUFFER, pbos[0]));
		GL_CHECK(glBufferData(GL_PIXEL_PACK_BUFFER, sizeof(int), 0, GL_STREAM_READ));

		GL_CHECK(glBindBuffer(GL_PIXEL_PACK_BUFFER, pbos[1]));
		GL_CHECK(glBufferData(GL_PIXEL_PACK_BUFFER, sizeof(int), 0, GL_STREAM_READ));

		GL_CHECK(glBindBuffer(GL_PIXEL_PACK_BUFFER, 0));

		renderer.Init();
		uiRenderer.Init();
	}

	void RenderSystem::Update(float deltaTime)
	{
		auto lights    = scene->CreateComponentGroup<DirectionalLightComponent>(GetTypeList<TransformComponent>);
		auto cameras   = scene->CreateComponentGroup<CameraComponent, TransformComponent>();
		auto cubemaps   = scene->CreateComponentView<CubemapComponent>();
		auto skyLights = scene->CreateComponentView<ProceduralSkylightComponent>();
		auto renderGroup = scene->CreateComponentGroup<RenderComponent>(GetTypeList<TransformComponent>);
		auto skinnedRenderGroup = scene->CreateComponentGroup<SkinnedRenderComponent>(GetTypeList<TransformComponent>);
		auto wireframeRenderGroup = scene->CreateComponentGroup<WireframeRenderComponent>(GetTypeList<TransformComponent>);
		auto spriteRenderGroup = scene->CreateComponentGroup<SpriteRenderComponent>(GetTypeList<TransformComponent>);
		auto textRenderGroup = scene->CreateComponentGroup<TextRenderComponent>(GetTypeList<TransformComponent>);

		DirectionalLightComponent light;
		TransformComponent lightTransform;
		if (!lights.empty())
		{
			light = lights.get<DirectionalLightComponent>(lights.front());
			lightTransform = lights.get<TransformComponent>(lights.front());
		}
		else
			light.color = glm::vec3(0, 0, 0);

		CameraComponent* cameraPointer;
		TransformComponent cameraTransform;
		if (!cameras.empty())
		{
			cameraPointer = &cameras.get<CameraComponent>(cameras.front());
			cameraTransform = cameras.get<TransformComponent>(cameras.front());
		}
		else
			cameraPointer = &defaultCamera;

		CameraComponent& camera = *cameraPointer;

		//TODO: Move to a better place
		{
			SoLoud::Soloud& soundEngine = *Application::Get()->GetSoundApi()->GetSoundEngine();
			soundEngine.set3dListenerPosition(cameraTransform.position.x, cameraTransform.position.y, cameraTransform.position.z);

			glm::vec3 listenerLookDirection = -cameraTransform.GetForwardDirection();
			soundEngine.set3dListenerAt(listenerLookDirection.x, listenerLookDirection.y, listenerLookDirection.z);
		}

		HdrCubemap* cubemap = resourceManager->GetAsset<HdrCubemap>("default_cubemap");
		if (!cubemaps.empty())
			cubemap = cubemaps.get<CubemapComponent>(cubemaps.front()).cubemap;

		if (!skyLights.empty())
		{
			ProceduralSkylightComponent& skyLight = skyLights.get<ProceduralSkylightComponent>(skyLights.front());

			if (!skyLight.cachedCubemap)
			{
				skyLight.cachedCubemap = new HdrCubemap();
				skyLight.cachedCubemap->CreateFromMaterial(skyLight.material);
			}

			if (skyLight.changed)
			{
				skyLight.cachedCubemap->Destroy();
				skyLight.cachedCubemap->CreateFromMaterial(skyLight.material);
				skyLight.changed = false;
			}
				
			cubemap = skyLight.cachedCubemap;
		}

		camera.aspectRatio = (float)framebufferWidth / framebufferHeight;

		//Shadow Pass
		//glDisable(GL_CULL_FACE);
		//glCullFace(GL_FRONT);
		GL_CHECK(glEnable(GL_DEPTH_CLAMP));

		glm::mat4 lightSpaceMatrices[CASCADE_COUNT];
		float farPlanes[CASCADE_COUNT] =
		{
			camera.farPlane / 24.0f, camera.farPlane / 7.0f, camera.farPlane / 2.0f, camera.farPlane
		};

		for (int i = 0; i < CASCADE_COUNT; i++)
		{
			depthFramebuffers[i].Bind();
			GL_CHECK(glViewport(0, 0, SHADOW_MAP_SIZE, SHADOW_MAP_SIZE));
			GL_CHECK(glClear(GL_DEPTH_BUFFER_BIT));
			
			float nearPlane, farPlane;
			if (i == 0)
			{
				nearPlane = camera.nearPlane;
				farPlane = farPlanes[i];
			}
			else if (i < CASCADE_COUNT)
			{
				nearPlane = farPlanes[i - 1];
				farPlane = farPlanes[i];
			}
			
			lightSpaceMatrices[i] = CalculateCsmMatrix(camera, cameraTransform, light, lightTransform, nearPlane, farPlane);

			depthShader->Use();
			depthShader->SetMat4("lightSpaceMatrix", lightSpaceMatrices[i]);

			skinnedDepthShader->Use();
			skinnedDepthShader->SetMat4("lightSpaceMatrix", lightSpaceMatrices[i]);

			renderer.Begin();
			
			static std::vector<Material*> ms, ms2;
			static Material m, m2;
			m.shader = depthShader;
			m2.shader = skinnedDepthShader;

			scene->Iterate
			(
				renderGroup,
				[&](EntityId id, RenderComponent& renderComponent, TransformComponent& transform)
				{
					Entity e = scene->GetEntityByEntityId(id);

					while (renderComponent.mesh->subMeshes.size() > ms.size())
						ms.push_back(&m);

					renderer.SubmitMesh(renderComponent.mesh, ms, e.GetGlobalTransformMatrix(), id);
				}
			);

			scene->Iterate
			(
				skinnedRenderGroup,
				[&](EntityId id, SkinnedRenderComponent& renderComponent, TransformComponent& transform)
				{
					Entity e = scene->GetEntityByEntityId(id);

					while (renderComponent.mesh->subMeshes.size() > ms2.size())
						ms2.push_back(&m2);

					renderer.SubmitSkinnedMesh(renderComponent.mesh, renderComponent.worldSpaceBoneTransforms, ms2, e.GetGlobalTransformMatrix(), id);
				}
			);

			renderer.Render();
			depthFramebuffers[i].Unbind();
		}

		renderer.End();

		GL_CHECK(glDisable(GL_DEPTH_CLAMP));

		//Hdr Pass
		GL_CHECK(glEnable(GL_CULL_FACE));
		hdrFramebuffer.Bind();

		GL_CHECK(glViewport(0, 0, framebufferWidth, framebufferHeight));
		GL_CHECK(glClearColor(0, 0, 0, 1.0f));
		GL_CHECK(glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT));
		GL_CHECK(glCullFace(GL_BACK));
		
		int clear = -1;
		GL_CHECK(glClearTexImage(entityMap.GetRenderId(), 0, GL_RED_INTEGER, GL_INT, &clear));

		unsigned int attachments[2] = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1 };
		GL_CHECK(glDrawBuffers(2, attachments));

		renderer.SetCamera
		(
			{
				cameraTransform.position,
				camera.GetViewMatrix(cameraTransform),
				camera.GetProjectionMatrix()
			}
		);

		renderer.SetDirectionalLight
		(
			{
				lightTransform.GetForwardDirection(),
				light.color* light.intensity
			}
		);

		renderer.SetShadowMaps
		(
			{
				shadowMaps,
				farPlanes,
				lightSpaceMatrices
			}
		);

		renderer.SetIBL(cubemap);
		renderer.SetTime(time);

		time += deltaTime;

		renderer.Begin();

		scene->Iterate
		(
			renderGroup,
			[&](EntityId id, RenderComponent& renderComponent, TransformComponent& transform)
			{
				Entity e = scene->GetEntityByEntityId(id);

				renderer.SubmitMesh(renderComponent.mesh, renderComponent.materials, e.GetGlobalTransformMatrix(), id);
			}
		);

		scene->Iterate
		(
			skinnedRenderGroup,
			[&](EntityId id, SkinnedRenderComponent& renderComponent, TransformComponent& transform)
			{
				Entity e = scene->GetEntityByEntityId(id);

				if (renderComponent.worldSpaceBoneTransforms.size() > 0) renderComponent.worldSpaceBoneTransforms[0] = renderComponent.boneTransforms[0];
				for (int i = 1; i < renderComponent.mesh->armature.bones.size(); i++)
				{
					BoneData& bone = renderComponent.mesh->armature.bones[i];
					renderComponent.worldSpaceBoneTransforms[i] = renderComponent.worldSpaceBoneTransforms[bone.parentId] * renderComponent.boneTransforms[i];
				}

				for (int i = 0; i < renderComponent.mesh->armature.bones.size(); i++)
				{
					BoneData& bone = renderComponent.mesh->armature.bones[i];
					renderComponent.worldSpaceBoneTransforms[i] *= bone.inverseBindPoseMatrix;
				}

				renderer.SubmitSkinnedMesh(renderComponent.mesh, renderComponent.worldSpaceBoneTransforms, renderComponent.materials, e.GetGlobalTransformMatrix(), id);
			}
		);

		scene->Iterate
		(
			wireframeRenderGroup,
			[&](EntityId id, WireframeRenderComponent& renderComponent, TransformComponent& transform)
			{
				Entity e = scene->GetEntityByEntityId(id);

				renderer.SubmitMeshWireframe(renderComponent.mesh, renderComponent.color, e.GetGlobalTransformMatrix(), id);
			}
		);

		scene->Iterate
		(
			textRenderGroup,
			[&](EntityId id, TextRenderComponent& renderComponent, TransformComponent& transform)
			{
				Entity e = scene->GetEntityByEntityId(id);

				renderer.SubmitText(renderComponent.text, renderComponent.font, renderComponent.color, e.GetGlobalTransformMatrix(), renderComponent.shadowDistance, renderComponent.shadowColor, id);
			}
		);

		scene->Iterate
		(
			spriteRenderGroup,
			[&](EntityId id, SpriteRenderComponent& renderComponent, TransformComponent& transform)
			{
				Entity e = scene->GetEntityByEntityId(id);

				renderer.SubmitSprite(renderComponent.sprite, renderComponent.tint, e.GetGlobalTransformMatrix(), id);
			}
		);

		for (RenderFunction& f : mainPassFunctions)
			f(renderer);

		renderer.Render();
		renderer.End();

		stats = renderer.GetRenderStats();

		uiRenderer.Begin();

		float aspectRatio = (float)framebufferWidth / framebufferHeight;
		float frameHalfSize = 100;
		glm::mat4 projectionMatrix = glm::ortho(-frameHalfSize * aspectRatio, frameHalfSize * aspectRatio, -frameHalfSize, frameHalfSize, -10.0f, 10.0f);
		uiRenderer.SetCamera
		(
			{
				glm::vec3(0),
				glm::identity<glm::mat4>(),
				projectionMatrix
			}
		);

		glm::vec3 anchorPositions[3] = { {0, 0, 0 }, { frameHalfSize * aspectRatio, 0, 0 }, { -frameHalfSize * aspectRatio, 0, 0 } };

		scene->CreateGroupAndIterate<UIAnchorComponent>
			(
				GetTypeList<TransformComponent>,
				[&](EntityId id, UIAnchorComponent& anchorComponent, TransformComponent& transform)
				{
					Entity e = scene->GetEntityByEntityId(id);

					if (anchorComponent.anchorPoint >= 0 && anchorComponent.anchorPoint < 3)
						transform.position = anchorPositions[anchorComponent.anchorPoint];
				}
		);

		scene->CreateViewAndIterate<UITextComponent>
			(
				[&](EntityId id, UITextComponent& textComponent)
				{
					Entity e = scene->GetEntityByEntityId(id);

					uiRenderer.SubmitText(textComponent.text, textComponent.font, textComponent.color, e.GetGlobalTransformMatrix(), textComponent.shadowDistance, textComponent.shadowColor, id);
				}
		);

		scene->CreateViewAndIterate<UISpriteComponent>
			(
				[&](EntityId id, UISpriteComponent& spriteComponent)
				{
					Entity e = scene->GetEntityByEntityId(id);

					uiRenderer.SubmitSprite(spriteComponent.sprite, spriteComponent.tint, e.GetGlobalTransformMatrix(), id);
				}
		);
		uiRenderer.End();
		uiRenderer.Render();

		ProcessMouseSelection();
		
		GL_CHECK(glDepthFunc(GL_LEQUAL));
		cubemapShader->Use();
		cubemapShader->SetMat4("viewMatrix", glm::mat4(glm::mat3(camera.GetViewMatrix(cameraTransform))));
		cubemapShader->SetMat4("projectionMatrix", camera.GetProjectionMatrix());

		cubemap->BindSkybox();

		captureCube.Draw();
		GL_CHECK(glDepthFunc(GL_LESS));

		hdrFramebuffer.Unbind();

		// Final Quad
		if(!renderToScreen) renderFramebuffer.Bind();
		GL_CHECK(glViewport(0, 0, framebufferWidth, framebufferHeight));
		GL_CHECK(glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT));

		quadShader->Use();
		quadShader->SetFloat("exposure", camera.exposure);

		hdrMap.Bind(0);
		fullscreenQuad.Draw();
		
		if (!renderToScreen) renderFramebuffer.Unbind();
	}

	void RenderSystem::Destroy()
	{
		window->removeWindowSizeCallback(windowResizeCallbackPosition);

		renderer.Destroy();
		uiRenderer.Destroy();
	}

	void RenderSystem::ResizeFramebuffer(unsigned int width, unsigned int height)
	{
		SD_ASSERT(initialized, "RenderSystem not initialized");

		if (width == framebufferWidth && height == framebufferHeight) return;

		if (width == 0 && height == 0)
		{
			useFullWindow = true;
			width = window->GetWidth();
			height = window->GetHeight();
		}
		else
			useFullWindow = false;

		framebufferWidth = width;
		framebufferHeight = height;

		hdrMap.Destroy();
		hdrMap.Create(width, height, (unsigned char*)NULL, TextureFormat::RGBA, TextureFormat::FLOAT16_ALPHA);

		entityMap.Destroy();
		entityMap.Create(width, height, (int*)NULL, TextureFormat::RED_INTEGER, TextureFormat::INT32);
		
		hdrDepthStencilBuffer.Destroy();
		hdrDepthStencilBuffer.Create(width, height, RenderBufferType::DEPTH_STENCIL);

		hdrFramebuffer.SetColorTexture(hdrMap);
		hdrFramebuffer.SetColorTexture(entityMap, 1);
		hdrFramebuffer.SetDepthStencilRenderBuffer(hdrDepthStencilBuffer);

		renderTarget.Destroy();
		renderTarget.Create(width, height, (unsigned char*)NULL, TextureFormat::RGBA, TextureFormat::RGBA);

		renderDepthStencilBuffer.Destroy();
		renderDepthStencilBuffer.Create(width, height, RenderBufferType::DEPTH_STENCIL);

		renderFramebuffer.SetColorTexture(renderTarget);
		renderFramebuffer.SetDepthStencilRenderBuffer(renderDepthStencilBuffer);
	}

	std::vector<glm::vec4> RenderSystem::CalculateFrustumCorners(CameraComponent& camera, TransformComponent& cameraTransform, float nearPlane, float farPlane)
	{
		glm::mat4 projectionMatrix = glm::perspective(camera.fov, camera.aspectRatio, nearPlane, farPlane);
		const glm::mat4& inv = glm::inverse(projectionMatrix * camera.GetViewMatrix(cameraTransform));

		std::vector<glm::vec4> frustumCorners;
		for (int x = 0; x < 2; x++)
			for (int y = 0; y < 2; y++)
				for (int z = 0; z < 2; z++)
				{
					const glm::vec4 point = inv *
						glm::vec4
						(
							2.0f * x - 1.0f,
							2.0f * y - 1.0f,
							2.0f * z - 1.0f,
							1.0f
						);

					frustumCorners.push_back(point / point.w);
				}

		return frustumCorners;
	}

	glm::mat4 RenderSystem::CalculateCsmMatrix(CameraComponent& camera, TransformComponent& cameraTransform, 
		DirectionalLightComponent& light, TransformComponent& lightTransform, float nearPlane, float farPlane)
	{
		const std::vector<glm::vec4>& corners = CalculateFrustumCorners(camera, cameraTransform, nearPlane, farPlane);

		glm::vec3 frustumCenter = glm::vec3(0, 0, 0);

		for (const auto& v : corners)
			frustumCenter += glm::vec3(v);

		frustumCenter /= corners.size();

		const glm::mat4 lightView = glm::lookAt(
			frustumCenter + lightTransform.GetForwardDirection(),
			frustumCenter,
			glm::vec3(0.0f, 1.0f, 0.0f)
		);

		float minX = std::numeric_limits<float>::max();
		float maxX = std::numeric_limits<float>::min();
		float minY = std::numeric_limits<float>::max();
		float maxY = std::numeric_limits<float>::min();
		float minZ = std::numeric_limits<float>::max();
		float maxZ = std::numeric_limits<float>::min();

		for (const glm::vec4& corner : corners)
		{
			const auto transformedCorner = lightView * corner;
			minX = std::min(minX, transformedCorner.x);
			maxX = std::max(maxX, transformedCorner.x);
			minY = std::min(minY, transformedCorner.y);
			maxY = std::max(maxY, transformedCorner.y);
			minZ = std::min(minZ, transformedCorner.z);
			maxZ = std::max(maxZ, transformedCorner.z);
		}
		
		constexpr float zMult = 10.0f;
		if (minZ < 0)
			minZ *= zMult;
		else
			minZ /= zMult;

		if (maxZ < 0)
			maxZ /= zMult;
		else
			maxZ *= zMult;
		
		const glm::mat4 lightProjection = glm::ortho(minX, maxX, minY, maxY, minZ, maxZ);
		return lightProjection * lightView;
	}

	void RenderSystem::ProcessMouseSelection()
	{
		static EntityId previousEntity = NullEntityId;
		static int index = 0;

		if (inputManager->GetMousePosition().x < framebufferWidth &&
			inputManager->GetMousePosition().x > 0 &&
			inputManager->GetMousePosition().y < framebufferHeight &&
			inputManager->GetMousePosition().y > 0)
		{
			index = (index + 1) % 2;
			int nextIndex = (index + 1) % 2;

			GL_CHECK(glReadBuffer(GL_COLOR_ATTACHMENT1));
			GL_CHECK(glBindBuffer(GL_PIXEL_PACK_BUFFER, pbos[index]));

			GL_CHECK(glReadPixels(inputManager->GetMousePosition().x, inputManager->GetMousePosition().y, 1, 1, GL_RED_INTEGER, GL_INT, 0));

			GL_CHECK(glBindBuffer(GL_PIXEL_PACK_BUFFER, pbos[nextIndex]));
			int* buffer = (int*)GL_CHECK(glMapBuffer(GL_PIXEL_PACK_BUFFER, GL_READ_ONLY));

			if (!buffer)
			{
				std::cerr << "Error reading pbo" << std::endl;
				return;
			}

			mouseSelectedEntity = (EntityId)*buffer;

			GL_CHECK(glUnmapBuffer(GL_PIXEL_PACK_BUFFER));
			GL_CHECK(glBindBuffer(GL_PIXEL_PACK_BUFFER, 0));

			if (scene->GetRegistry().valid(mouseSelectedEntity))
			{
				MouseSelectionComponent& m = scene->GetRegistry().get<MouseSelectionComponent>(mouseSelectedEntity);

				if (inputManager->GetMouseButtonPressed(MouseButton::LEFT)) m.status = SelectionStatus::CLICKED;
				else if (inputManager->GetMouseButtonDown(MouseButton::LEFT)) m.status = SelectionStatus::HELD;
				else m.status = SelectionStatus::HOVERED;

			}

			if (previousEntity != mouseSelectedEntity && scene->GetRegistry().valid(previousEntity))
			{
				MouseSelectionComponent& m = scene->GetRegistry().get<MouseSelectionComponent>(previousEntity);
				m.status = SelectionStatus::NONE;
			}

			previousEntity = mouseSelectedEntity;
		}
	}

	void RenderSystem::AddMainRenderPassFunction(const RenderFunction& function)
	{
		mainPassFunctions.push_back(function);
	}
}