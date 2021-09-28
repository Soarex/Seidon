#pragma once
#include <glad/glad.h>

#include "Core/System.h"
#include "Core/ResourceManager.h"
#include "Core/Window.h"
#include "Core/Entity.h"
#include "Shader.h"
#include "Framebuffer.h"

namespace Seidon
{
	class RenderSystem : public System
	{
	private:
		std::function<void(int, int)> windowResizeCallback;

		Shader shader;
		Shader depthShader;
		Shader cubemapShader;
		Shader quadShader;

		Framebuffer hdrFramebuffer;
		Framebuffer depthFramebuffer;
		Framebuffer renderFramebuffer;

		Texture hdrMap;
		Texture bloomMap;
		Texture shadowMap;

		Texture renderTarget;

		RenderBuffer hdrDepthStencilBuffer;
		RenderBuffer renderDepthStencilBuffer;

		SubMesh* fullscreenQuad;

		CameraComponent defaultCamera;

		bool useFullWindow = true;
		unsigned int framebufferWidth, framebufferHeight;

		const int SHADOW_MAP_SIZE = 1024;
	public:
		RenderSystem();
		void Init();
		void Update(float deltatime);
		void Destroy();

		void ResizeFramebuffer(unsigned int width, unsigned int height);

		inline const Texture& GetRenderTarget() { return renderTarget; }


	};
}