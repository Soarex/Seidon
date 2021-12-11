#include "RenderSystem.h"
#include "../Debug/Debug.h"

#include "Ecs/Scene.h"

#include "Core/Application.h"

namespace Seidon
{

	void RenderSystem::Init()
	{
		framebufferWidth = window->GetWidth();
		framebufferHeight = window->GetHeight();

		captureCube.Init();

		hdrMap.Create(window->GetWidth(), window->GetHeight(), (unsigned char*)NULL, TextureFormat::RGBA, TextureFormat::FLOAT16_ALPHA);
		hdrDepthStencilBuffer.Create(window->GetWidth(), window->GetHeight(), RenderBufferType::DEPTH_STENCIL);

		hdrFramebuffer.Create();
		hdrFramebuffer.SetColorTexture(hdrMap);
		hdrFramebuffer.SetDepthStencilRenderBuffer(hdrDepthStencilBuffer);

		int shadowSamplers[CASCADE_COUNT];
		for (int i = 0; i < CASCADE_COUNT; i++)
		{
			shadowMaps[i].Create(SHADOW_MAP_SIZE, SHADOW_MAP_SIZE, (unsigned char*)NULL, TextureFormat::DEPTH, TextureFormat::DEPTH, ClampingMode::BORDER);
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

		shader.LoadFromFile("Shaders/PBR.shader");
		shader.Use();
		shader.SetInt("albedoMap", 0);
		shader.SetInt("roughnessMap", 1);
		shader.SetInt("normalMap", 2);
		shader.SetInt("metallicMap", 3);
		shader.SetInt("aoMap", 4);
		shader.SetInt("irradianceMap", 5);
		shader.SetInt("prefilterMap", 6);
		shader.SetInt("BRDFLookupMap", 7);

		shader.SetInts("shadowMaps", shadowSamplers, CASCADE_COUNT);

		depthShader.LoadFromFile("Shaders/ShadowPass.shader");
		quadShader.LoadFromFile("Shaders/Simple.shader");
		cubemapShader.LoadFromFile("Shaders/Cubemap.shader");

		std::vector<Vertex> quadVertices =
		{
			// positions			// normals			//tangents			 // UVs
			{{-1.0f,  1.0f, 0.0f},	{0.0f, 0.0f, 0.0f},	{0.0f, 0.0f, 0.0f},	 {0.0f, 1.0f} },
			{{-1.0f, -1.0f, 0.0f},	{0.0f, 0.0f, 0.0f},	{0.0f, 0.0f, 0.0f},	 {0.0f, 0.0f} },
			{{ 1.0f, -1.0f, 0.0f},	{0.0f, 0.0f, 0.0f},	{0.0f, 0.0f, 0.0f},	 {1.0f, 0.0f} },

			{{-1.0f,  1.0f, 0.0f},	{0.0f, 0.0f, 0.0f},	{0.0f, 0.0f, 0.0f},	 {0.0f, 1.0f} },
			{{ 1.0f, -1.0f, 0.0f},	{0.0f, 0.0f, 0.0f},	{0.0f, 0.0f, 0.0f},	 {1.0f, 0.0f} },
			{{ 1.0f,  1.0f, 0.0f},	{0.0f, 0.0f, 0.0f},	{0.0f, 0.0f, 0.0f},	 {1.0f, 1.0f} }
		};

		std::vector<unsigned int> indices = { 0, 1, 2, 3, 4, 5 };
		fullscreenQuad = new SubMesh();
		fullscreenQuad->Create(quadVertices, indices, "");

		GL_CHECK(glEnable(GL_DEPTH_TEST));
		GL_CHECK(glEnable(GL_BLEND));
		GL_CHECK(glEnable(GL_CULL_FACE));
		GL_CHECK(glEnable(GL_TEXTURE_CUBE_MAP_SEAMLESS));

		windowResizeCallbackPosition = 
			window->AddWindowSizeCallback([this](int width, int height)
			{
				if (useFullWindow && width > 0 && height > 0)
					ResizeFramebuffer(width, height);
			});

		renderFramebuffer.Unbind();
	}

	void RenderSystem::Update(float deltaTime)
	{
		entt::basic_group lights    = scene->GetRegistry().group<DirectionalLightComponent>(entt::get<TransformComponent>);
		entt::basic_group cameras   = scene->GetRegistry().group<CameraComponent, TransformComponent>();
		entt::basic_view cubemaps   = scene->GetRegistry().view<CubemapComponent>();
		entt::basic_group renderGroup = scene->GetRegistry().group<RenderComponent>(entt::get<TransformComponent>);

		DirectionalLightComponent light;
		TransformComponent lightTransform;
		if (!lights.empty())
		{
			light = lights.get<DirectionalLightComponent>(lights.front());
			lightTransform = lights.get<TransformComponent>(lights.front());
		}
		else
			light.color = glm::vec3(0, 0, 0);

		CameraComponent camera;
		TransformComponent cameraTransform;
		if (!cameras.empty())
		{
			camera = cameras.get<CameraComponent>(cameras.front());
			cameraTransform = cameras.get<TransformComponent>(cameras.front());
		}
		else
			camera = defaultCamera;

		CubemapComponent cubemap;
		if (!cubemaps.empty())
			cubemap = cubemaps.get<CubemapComponent>(cubemaps.front());

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

			depthShader.Use();
			depthShader.SetMat4("lightSpaceMatrix", lightSpaceMatrices[i]);

			for (entt::entity e : renderGroup)
			{
				RenderComponent r = renderGroup.get<RenderComponent>(e);
				TransformComponent t = renderGroup.get<TransformComponent>(e);
				glm::mat4 modelMatrix = t.GetTransformMatrix();
				depthShader.SetMat4("modelMatrix", modelMatrix);

				for (SubMesh* subMesh : r.mesh->subMeshes)
				{
					GL_CHECK(glBindVertexArray(subMesh->GetVAO()));
					GL_CHECK(glDrawElements(GL_TRIANGLES, subMesh->indices.size(), GL_UNSIGNED_INT, 0));
				}
			}

			depthFramebuffers[i].Unbind();
		}

		GL_CHECK(glDisable(GL_DEPTH_CLAMP));
		//Hdr Pass
		GL_CHECK(glEnable(GL_CULL_FACE));
		hdrFramebuffer.Bind();

		GL_CHECK(glViewport(0, 0, framebufferWidth, framebufferHeight));
		GL_CHECK(glClearColor(0.2f, 0.3f, 0.3f, 1.0f));
		GL_CHECK(glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT));
		GL_CHECK(glCullFace(GL_BACK));

		shader.Use();
		shader.SetVec3("directionalLight.direction", lightTransform.GetForwardDirection());
		shader.SetVec3("directionalLight.color", light.color * light.intensity);

		shader.SetMat4("viewMatrix", camera.GetViewMatrix(cameraTransform));
		shader.SetMat4("projectionMatrix", camera.GetProjectionMatrix());

		shader.SetVec3("cameraPosition", cameraTransform.position);

		cubemap.cubemap->BindIrradianceMap(5);
		cubemap.cubemap->BindPrefilteredMap(6);
		cubemap.cubemap->BindBRDFLookupMap(7);

		for (int i = 0; i < CASCADE_COUNT; i++)
		{
			shader.SetMat4("lightSpaceMatrices[" + std::to_string(i) + "]", lightSpaceMatrices[i]);
			shader.SetFloat("cascadeFarPlaneDistances[" + std::to_string(i) + "]", farPlanes[i]);
			shader.SetInt("cascadeCount", CASCADE_COUNT);
			shadowMaps[i].Bind(8 + i);
		}

		for (entt::entity e : renderGroup)
		{
			RenderComponent& r = renderGroup.get<RenderComponent>(e);
			TransformComponent& t = renderGroup.get<TransformComponent>(e);

			glm::mat4 modelMatrix = t.GetTransformMatrix();

			shader.SetMat4("modelMatrix", modelMatrix);

			glm::mat3 normalMatrix = glm::mat3(glm::transpose(glm::inverse(modelMatrix)));
			shader.SetMat3("normalMatrix", normalMatrix);

			int i = 0;
			for (SubMesh* subMesh : r.mesh->subMeshes)
			{
				if (r.materials.size() <= i) r.materials.push_back(resourceManager->GetMaterial("default_material"));

				shader.SetVec3("tint", r.materials[i]->tint);
				r.materials[i]->albedo->Bind(0);
				r.materials[i]->roughness->Bind(1);
				r.materials[i]->normal->Bind(2);
				r.materials[i]->metallic->Bind(3);
				r.materials[i]->ao->Bind(4);

				GL_CHECK(glBindVertexArray(subMesh->GetVAO()));
				
				GL_CHECK(glDrawElements(GL_TRIANGLES, subMesh->indices.size(), GL_UNSIGNED_INT, 0));

				i++;
			}
		}

		GL_CHECK(glDepthFunc(GL_LEQUAL));
		cubemapShader.Use();
		cubemapShader.SetMat4("viewMatrix", glm::mat4(glm::mat3(camera.GetViewMatrix(cameraTransform))));
		cubemapShader.SetMat4("projectionMatrix", camera.GetProjectionMatrix());
		cubemap.cubemap->BindSkybox();
		captureCube.Draw();
		GL_CHECK(glDepthFunc(GL_LESS));

		hdrFramebuffer.Unbind();

		// Final Quad
		if(!renderToScreen) renderFramebuffer.Bind();
		GL_CHECK(glViewport(0, 0, framebufferWidth, framebufferHeight));
		GL_CHECK(glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT));

		quadShader.Use();
		quadShader.SetFloat("exposure", camera.exposure);

		hdrMap.Bind(0);
		
		GL_CHECK(glBindVertexArray(fullscreenQuad->GetVAO()));
		GL_CHECK(glDrawElements(GL_TRIANGLES, fullscreenQuad->indices.size(), GL_UNSIGNED_INT, 0));
		if (!renderToScreen) renderFramebuffer.Unbind();
	}

	void RenderSystem::Destroy()
	{
		delete fullscreenQuad;
		window->removeWindowSizeCallback(windowResizeCallbackPosition);
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
		
		hdrDepthStencilBuffer.Destroy();
		hdrDepthStencilBuffer.Create(width, height, RenderBufferType::DEPTH_STENCIL);

		hdrFramebuffer.SetColorTexture(hdrMap);
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
}