#pragma once
#include <glad/glad.h>

#include "Core/ResourceManager.h"
#include "Core/Window.h"
#include "Ecs/System.h"
#include "Ecs/Entity.h"
#include "Shader.h"
#include "Framebuffer.h"

namespace Seidon
{
	class RenderSystem : public System
	{
	private:
		constexpr static int SHADOW_MAP_SIZE = 1024;
		constexpr static int CASCADE_COUNT = 4;

		std::function<void(int, int)> windowResizeCallback;

		Shader shader;
		Shader depthShader;
		Shader cubemapShader;
		Shader quadShader;

		Framebuffer hdrFramebuffer;
		Framebuffer depthFramebuffers[CASCADE_COUNT];
		Framebuffer renderFramebuffer;

		Texture hdrMap;
		Texture bloomMap;
		
		Texture shadowMaps[CASCADE_COUNT];

		Texture renderTarget;

		RenderBuffer hdrDepthStencilBuffer;
		RenderBuffer renderDepthStencilBuffer;

		SubMesh* fullscreenQuad;

		CameraComponent defaultCamera;

		bool useFullWindow = true;
		unsigned int framebufferWidth, framebufferHeight;

	public:
		RenderSystem();
		void Init();
		void Update(float deltatime);
		void Destroy();

		void ResizeFramebuffer(unsigned int width, unsigned int height);

		inline const Texture& GetRenderTarget() { return renderTarget; }

	private:
		std::vector<glm::vec4> CalculateFrustumCorners(CameraComponent& camera, TransformComponent& cameraTransform, float nearPlane, float farPlane);

		glm::mat4 CalculateCsmMatrix(CameraComponent& camera, TransformComponent& cameraTransform, 
			DirectionalLightComponent& light, TransformComponent& lightTransform, float nearPlane, float farPlane);
	};
}