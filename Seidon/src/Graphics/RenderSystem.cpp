#include "RenderSystem.h"
#include "Core/Scene.h"

namespace Seidon
{
	void DrawCaptureCube()
	{
		const float cubeVertices[] = {
			// positions          
			-1.0f,  1.0f, -1.0f,
			-1.0f, -1.0f, -1.0f,
			 1.0f, -1.0f, -1.0f,
			 1.0f, -1.0f, -1.0f,
			 1.0f,  1.0f, -1.0f,
			-1.0f,  1.0f, -1.0f,

			-1.0f, -1.0f,  1.0f,
			-1.0f, -1.0f, -1.0f,
			-1.0f,  1.0f, -1.0f,
			-1.0f,  1.0f, -1.0f,
			-1.0f,  1.0f,  1.0f,
			-1.0f, -1.0f,  1.0f,

			 1.0f, -1.0f, -1.0f,
			 1.0f, -1.0f,  1.0f,
			 1.0f,  1.0f,  1.0f,
			 1.0f,  1.0f,  1.0f,
			 1.0f,  1.0f, -1.0f,
			 1.0f, -1.0f, -1.0f,

			-1.0f, -1.0f,  1.0f,
			-1.0f,  1.0f,  1.0f,
			 1.0f,  1.0f,  1.0f,
			 1.0f,  1.0f,  1.0f,
			 1.0f, -1.0f,  1.0f,
			-1.0f, -1.0f,  1.0f,

			-1.0f,  1.0f, -1.0f,
			 1.0f,  1.0f, -1.0f,
			 1.0f,  1.0f,  1.0f,
			 1.0f,  1.0f,  1.0f,
			-1.0f,  1.0f,  1.0f,
			-1.0f,  1.0f, -1.0f,

			-1.0f, -1.0f, -1.0f,
			-1.0f, -1.0f,  1.0f,
			 1.0f, -1.0f, -1.0f,
			 1.0f, -1.0f, -1.0f,
			-1.0f, -1.0f,  1.0f,
			 1.0f, -1.0f,  1.0f
		};

		unsigned int cubeVAO = 0;
		unsigned int cubeVBO = 0;
		glGenVertexArrays(1, &cubeVAO);
		glGenBuffers(1, &cubeVBO);

		glBindBuffer(GL_ARRAY_BUFFER, cubeVBO);
		glBufferData(GL_ARRAY_BUFFER, sizeof(cubeVertices), cubeVertices, GL_STATIC_DRAW);

		glBindVertexArray(cubeVAO);
		glEnableVertexAttribArray(0);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);

		glBindBuffer(GL_ARRAY_BUFFER, 0);
		glBindVertexArray(0);

		glBindVertexArray(cubeVAO);
		glDrawArrays(GL_TRIANGLES, 0, 36);
		glBindVertexArray(0);
	}

	RenderSystem::RenderSystem()
	{
	}

	void RenderSystem::Init()
	{
		framebufferWidth = Window::GetWidth();
		framebufferHeight = Window::GetHeight();

		hdrMap.Create(Window::GetWidth(), Window::GetHeight(), (unsigned char*)NULL, TextureFormat::FLOAT16_ALPHA, TextureFormat::RGBA);
		hdrDepthStencilBuffer.Create(Window::GetWidth(), Window::GetHeight(), RenderBufferType::DEPTH_STENCIL);

		hdrFramebuffer.Create();
		hdrFramebuffer.SetColorTexture(hdrMap);
		hdrFramebuffer.SetDepthStencilRenderBuffer(hdrDepthStencilBuffer);

		shadowMap.Create(SHADOW_MAP_SIZE, SHADOW_MAP_SIZE, (unsigned char*)NULL, TextureFormat::DEPTH, TextureFormat::DEPTH, ClampingMode::BORDER);

		depthFramebuffer.Create();
		depthFramebuffer.DisableColorBuffer();
		depthFramebuffer.SetDepthTexture(shadowMap);

		renderTarget.Create(Window::GetWidth(), Window::GetHeight(), (unsigned char*)NULL, TextureFormat::RGBA, TextureFormat::RGBA);
		renderDepthStencilBuffer.Create(Window::GetWidth(), Window::GetHeight(), RenderBufferType::DEPTH_STENCIL);
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
		shader.SetInt("shadowMap", 5);
		shader.SetInt("irradianceMap", 6);
		shader.SetInt("prefilterMap", 7);
		shader.SetInt("BRDFLookupMap", 8);

		depthShader.LoadFromFileAsync("Shaders/ShadowPass.shader");
		quadShader.LoadFromFileAsync("Shaders/Simple.shader");
		cubemapShader.LoadFromFileAsync("Shaders/Cubemap.shader");

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
		fullscreenQuad->Create(quadVertices, indices, NULL);

		glEnable(GL_DEPTH_TEST);
		glEnable(GL_BLEND);
		glEnable(GL_CULL_FACE);
		glEnable(GL_TEXTURE_CUBE_MAP_SEAMLESS);

		windowResizeCallback = [this](int width, int height)
		{
			if (useFullWindow)
				ResizeFramebuffer(width, height);
		};

		Window::AddWindowSizeCallback(windowResizeCallback);

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
		glViewport(0, 0, SHADOW_MAP_SIZE, SHADOW_MAP_SIZE);
		depthFramebuffer.Bind();
		glClear(GL_DEPTH_BUFFER_BIT);
		glDisable(GL_CULL_FACE);
		//glCullFace(GL_FRONT);

		
		glm::mat4 lightView = glm::lookAt(lightTransform.GetForwardDirection(),
			glm::vec3(0.0f, 0.0f, 0.0f),
			glm::vec3(0.0f, 1.0f, 0.0f));

		float near_plane = -100.0f, far_plane = 100.0f;

		glm::mat4 lightProjection = glm::ortho(-100.0f, 100.0f, -100.0f, 100.0f, near_plane, far_plane);
		glm::mat4 lightSpaceMatrix = lightProjection * lightView;
		
		
		/*
		//Shadow bounds calulation
		glm::mat4 inverseViewMatrix = glm::inverse(camera.GetViewMatrix(cameraTransform));
		glm::mat4 lightViewMatrix = glm::lookAt(lightTransform.GetForwardDirection(),
			glm::vec3(0.0f, 0.0f, 0.0f),
			glm::vec3(0.0f, 1.0f, 0.0f));

		float tanHalfHFOV = tanf(glm::radians(camera.fov / 2.0f));
		float tanHalfVFOV = tanf(glm::radians((camera.fov * camera.aspectRatio) / 2.0f));

		float xn = camera.nearPlane * tanHalfHFOV;
		float xf = camera.farPlane * tanHalfHFOV;
		float yn = camera.nearPlane * tanHalfVFOV;
		float yf = camera.farPlane * tanHalfVFOV;

		glm::vec4 frustumCorners[8] = {
			// near face
			glm::vec4(xn, yn, camera.nearPlane, 1.0),
			glm::vec4(-xn, yn, camera.nearPlane, 1.0),
			glm::vec4(xn, -yn, camera.nearPlane, 1.0),
			glm::vec4(-xn, -yn, camera.nearPlane, 1.0),

			// far face
			glm::vec4(xf, yf, camera.farPlane, 1.0),
			glm::vec4(-xf, yf, camera.farPlane, 1.0),
			glm::vec4(xf, -yf, camera.farPlane, 1.0),
			glm::vec4(-xf, -yf, camera.farPlane, 1.0)
		};

		float minX = std::numeric_limits<int>::max();
		float maxX = std::numeric_limits<int>::min();
		float minY = std::numeric_limits<int>::max();
		float maxY = std::numeric_limits<int>::min();
		float minZ = std::numeric_limits<int>::max();
		float maxZ = std::numeric_limits<int>::min();
		
		glm::vec4 frustumCornersL[8];
		for (unsigned int j = 0; j < 8; j++) {

			glm::vec4 vW = inverseViewMatrix * frustumCorners[j];

			frustumCornersL[j] = lightViewMatrix * vW;

			minX = std::min(minX, frustumCornersL[j].x);
			maxX = std::max(maxX, frustumCornersL[j].x);
			minY = std::min(minY, frustumCornersL[j].y);
			maxY = std::max(maxY, frustumCornersL[j].y);
			minZ = std::min(minZ, frustumCornersL[j].z);
			maxZ = std::max(maxZ, frustumCornersL[j].z);
			
		}

		ImGui::Text("%f, %f, %f, %f, %f, %f", minX, maxX, minY, maxY, minZ, maxZ);
		glm::mat4 lightProjectionMatrix = glm::ortho(minX, maxX, minY, maxY, minZ, maxZ);
		glm::mat4 lightSpaceMatrix = lightProjectionMatrix * lightViewMatrix;
		*/
		depthShader.Use();
		depthShader.SetMat4("lightSpaceMatrix", lightSpaceMatrix);

		for (entt::entity e : renderGroup)
		{
			RenderComponent r = renderGroup.get<RenderComponent>(e);
			TransformComponent t = renderGroup.get<TransformComponent>(e);
			glm::mat4 modelMatrix = t.GetTransformMatrix();
			depthShader.SetMat4("modelMatrix", modelMatrix);

			for (int i = 0; i < r.mesh->subMeshes.size(); i++)
			{
				glBindVertexArray(r.mesh->subMeshes[i]->GetVAO());

				glDrawElements(GL_TRIANGLES, r.mesh->subMeshes[i]->indices.size(), GL_UNSIGNED_INT, 0);
			}
		}

		depthFramebuffer.Unbind();

		//Hdr Pass
		glEnable(GL_CULL_FACE);
		hdrFramebuffer.Bind();

		glViewport(0, 0, framebufferWidth, framebufferHeight);
		glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		glCullFace(GL_BACK);

		shader.Use();
		shader.SetVec3("directionalLight.direction", lightTransform.GetForwardDirection());
		shader.SetVec3("directionalLight.color", light.color * light.intensity);

		shader.SetMat4("viewMatrix", camera.GetViewMatrix(cameraTransform));
		shader.SetMat4("projectionMatrix", camera.GetProjectionMatrix());
		shader.SetMat4("lightSpaceMatrix", lightSpaceMatrix);
		shader.SetVec3("cameraPosition", cameraTransform.position);

		shadowMap.Bind(5);
		cubemap.cubemap->BindIrradianceMap(6);
		cubemap.cubemap->BindPrefilteredMap(7);
		cubemap.cubemap->BindBRDFLookupMap(8);

		for (entt::entity e : renderGroup)
		{
			RenderComponent r = renderGroup.get<RenderComponent>(e);
			TransformComponent t = renderGroup.get<TransformComponent>(e);

			glm::mat4 modelMatrix = t.GetTransformMatrix();

			shader.SetMat4("modelMatrix", modelMatrix);

			glm::mat3 normalMatrix = glm::mat3(glm::transpose(glm::inverse(modelMatrix)));
			shader.SetMat3("normalMatrix", normalMatrix);

			for (SubMesh* subMesh : r.mesh->subMeshes)
			{
				shader.SetVec3("tint", subMesh->material->tint);
				subMesh->material->albedo->Bind(0);
				subMesh->material->roughness->Bind(1);
				subMesh->material->normal->Bind(2);
				subMesh->material->metallic->Bind(3);
				subMesh->material->ao->Bind(4);

				glBindVertexArray(subMesh->GetVAO());

				glDrawElements(GL_TRIANGLES, subMesh->indices.size(), GL_UNSIGNED_INT, 0);
			}
		}

		glDepthFunc(GL_LEQUAL);
		cubemapShader.Use();
		cubemapShader.SetMat4("viewMatrix", glm::mat4(glm::mat3(camera.GetViewMatrix(cameraTransform))));
		cubemapShader.SetMat4("projectionMatrix", camera.GetProjectionMatrix());
		cubemap.cubemap->BindSkybox();
		DrawCaptureCube();
		glDepthFunc(GL_LESS);

		hdrFramebuffer.Unbind();

		// Final Quad
		renderFramebuffer.Bind();
		glViewport(0, 0, framebufferWidth, framebufferHeight);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		quadShader.Use();
		quadShader.SetFloat("exposure", camera.exposure);

		hdrMap.Bind(0);

		glBindVertexArray(fullscreenQuad->GetVAO());
		glDrawElements(GL_TRIANGLES, fullscreenQuad->indices.size(), GL_UNSIGNED_INT, 0);
		renderFramebuffer.Unbind();
	}

	void RenderSystem::Destroy()
	{
		delete fullscreenQuad;
	}

	void RenderSystem::ResizeFramebuffer(unsigned int width, unsigned int height)
	{
		if (width == framebufferWidth && height == framebufferHeight) return;

		if (width == 0 && height == 0)
		{
			useFullWindow = true;
			width = Window::GetWidth();
			height = Window::GetHeight();
		}
		else
			useFullWindow = false;

		framebufferWidth = width;
		framebufferHeight = height;

		hdrMap.Destroy();
		hdrMap.Create(width, height, (unsigned char*)NULL, TextureFormat::FLOAT16_ALPHA, TextureFormat::RGBA);
		
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
}