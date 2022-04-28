#pragma once
#include <glad/glad.h>

#include "Core/ResourceManager.h"
#include "Core/Window.h"
#include "Ecs/System.h"
#include "Ecs/Entity.h"
#include "Renderer.h"
#include "Shader.h"
#include "Framebuffer.h"
#include "CaptureCube.h"

namespace Seidon
{
	class RenderSystem : public System
	{
	private:
		constexpr static int SHADOW_MAP_SIZE = 2048;
		constexpr static int CASCADE_COUNT = 4;

		std::list<std::function<void(int, int)>>::iterator windowResizeCallbackPosition;

		Renderer renderer;

		Shader* shader;
		Shader depthShader;
		Shader cubemapShader;
		Shader quadShader;

		Framebuffer hdrFramebuffer;
		Framebuffer depthFramebuffers[CASCADE_COUNT];
		Framebuffer renderFramebuffer;

		unsigned int pbos[2];

		Texture hdrMap;
		Texture entityMap;
		//Texture bloomMap;
		
		Texture shadowMaps[CASCADE_COUNT];

		Texture renderTarget;

		RenderBuffer hdrDepthStencilBuffer;
		RenderBuffer renderDepthStencilBuffer;

		SubMesh* fullscreenQuad;
		CaptureCube captureCube;

		CameraComponent defaultCamera;

		bool useFullWindow = true;
		bool renderToScreen = true;
		unsigned int framebufferWidth, framebufferHeight;

		entt::entity mouseSelectedEntity;
	public:
		RenderSystem() = default;
		~RenderSystem() = default;
		void Init() override;
		void Update(float deltatime) override;
		void Destroy() override;

		void ResizeFramebuffer(unsigned int width, unsigned int height);

		inline const Texture& GetRenderTarget() { return renderTarget; }
		inline entt::entity GetMouseSelectedEntity() { return mouseSelectedEntity; }
		inline void SetRenderToScreen(bool value) { renderToScreen = value; }

	private:
		std::vector<glm::vec4> CalculateFrustumCorners(CameraComponent& camera, TransformComponent& cameraTransform, float nearPlane, float farPlane);

		glm::mat4 CalculateCsmMatrix(CameraComponent& camera, TransformComponent& cameraTransform, 
			DirectionalLightComponent& light, TransformComponent& lightTransform, float nearPlane, float farPlane);

		void ProcessMouseSelection();
	};
}