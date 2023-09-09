#include "OpenGLTestLayer.h"

OpenGLTestLayer::OpenGLTestLayer() 
	: Layer("OpenGL TestLayer") 
{
	m_Shader.reset(Stimpi::Shader::CreateShader("shader.shader"));
	m_SceneCamera = std::make_shared<Stimpi::Camera>(1920.0f, 1080.0f);
	m_Texture.reset(Stimpi::Texture::CreateTexture("Capture.jpg"));
	m_Texture2.reset(Stimpi::Texture::CreateTexture("Picture1.jpg"));
}

void OpenGLTestLayer::OnAttach() 
{
	ST_TRACE("{0}: OnAttach", m_DebugName); 
}

void OpenGLTestLayer::OnDetach()
{ 
	ST_TRACE("{0}: OnDetach", m_DebugName); 
}

void OpenGLTestLayer::Update()
{
	// Test Scene render
	Stimpi::Renderer2D::Instace()->BeginScene(m_SceneCamera.get(), m_Shader.get());

	Stimpi::Renderer2D::Instace()->UseTexture(m_Texture.get());
	Stimpi::Renderer2D::Instace()->PushQuad(0.0f, 0.0f, 250.f, 200.0f, 1.0f, 1.0f);
	Stimpi::Renderer2D::Instace()->UseTexture(m_Texture2.get());
	Stimpi::Renderer2D::Instace()->PushQuad(550.0f, 550.0f, 250.f, 200.0f, 1.0f, 1.0f);
	Stimpi::Renderer2D::Instace()->UseTexture(m_Texture.get());
	Stimpi::Renderer2D::Instace()->PushQuad(850.0f, 850.0f, 250.f, 200.0f, 1.0f, 1.0f);

	Stimpi::Renderer2D::Instace()->EndScene();
}

void OpenGLTestLayer::OnEvent(Stimpi::BaseEvent* e)
{

}