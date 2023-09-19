#include "OpenGLTestLayer.h"

OpenGLTestLayer::OpenGLTestLayer() 
	: Layer("OpenGL TestLayer") 
{
	m_Shader.reset(Stimpi::Shader::CreateShader("shader.shader"));
	m_SceneCamera = std::make_shared<Stimpi::Camera>(1280.0f, 720.0f); //TODO debug, smth to do with screen size and sub window size...
	m_Texture.reset(Stimpi::Texture::CreateTexture("Capture.jpg"));
	m_Texture2.reset(Stimpi::Texture::CreateTexture("Picture1.jpg"));

	// Just for test, no need to use here since Layer::OnEvent exists
	m_KeyboardHandler = std::make_shared<Stimpi::KeyboardEventHandler>([](Stimpi::KeyboardEvent event) -> bool {
			ST_INFO("Handling KeyboardEvent key: {0}", event.GetKeyCode());
			return false;
		});
	//Register handler
	Stimpi::InputManager::Instance()->AddKeyboardEventHandler(m_KeyboardHandler.get());
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
	// Match camrea display area to framebuffer size
	auto canvasWidth = Stimpi::Renderer2D::Instace()->GetCanvasWidth();
	auto canvasHeight = Stimpi::Renderer2D::Instace()->GetCanvasHeight();

	m_SceneCamera->Resize(canvasWidth, canvasHeight);

	if (Stimpi::InputManager::IsKeyPressed(ST_KEY_I))
		m_SceneCamera->Translate({0.0f, 10.0f, 0.0f});
	if (Stimpi::InputManager::IsKeyPressed(ST_KEY_K))
		m_SceneCamera->Translate({ 0.0f, -10.0f, 0.0f });
	if (Stimpi::InputManager::IsKeyPressed(ST_KEY_L))
		m_SceneCamera->Translate({ 10.0f, 0.0f, 0.0f });
	if (Stimpi::InputManager::IsKeyPressed(ST_KEY_J))
		m_SceneCamera->Translate({ -10.0f, 0.0f, 0.0f });
	
	Stimpi::Renderer2D::Instace()->BeginScene(m_SceneCamera.get(), m_Shader.get());
	{
		Stimpi::Renderer2D::Instace()->UseTexture(m_Texture.get());
		Stimpi::Renderer2D::Instace()->PushQuad(0.0f, 0.0f, canvasWidth, canvasHeight, 1.0f, 1.0f);
		/*Stimpi::Renderer2D::Instace()->UseTexture(m_Texture.get());
		Stimpi::Renderer2D::Instace()->PushQuad(0.0f, 0.0f, 250.f, 200.0f, 1.0f, 1.0f);
		Stimpi::Renderer2D::Instace()->UseTexture(m_Texture2.get());
		Stimpi::Renderer2D::Instace()->PushQuad(550.0f, 550.0f, 250.f, 200.0f, 1.0f, 1.0f);
		Stimpi::Renderer2D::Instace()->UseTexture(m_Texture.get());
		Stimpi::Renderer2D::Instace()->PushQuad(750.0f, 850.0f, 250.f, 200.0f, 1.0f, 1.0f);*/
	}
	Stimpi::Renderer2D::Instace()->EndScene();


	/*Stimpi::Renderer2D::Instace()->BeginScene(m_SceneCamera.get(), m_Shader.get());
	{
		Stimpi::Renderer2D::Instace()->UseTexture(m_Texture.get());
		Stimpi::Renderer2D::Instace()->PushQuad(1000.0f, 500.0f, 680.f, 520.0f, 1.0f, 1.0f);
		Stimpi::Renderer2D::Instace()->UseTexture(m_Texture2.get());
		Stimpi::Renderer2D::Instace()->PushQuad(320.0f, 180.0f, 640.f, 360.0f, 1.0f, 1.0f);
	}
	Stimpi::Renderer2D::Instace()->EndScene();*/
}

void OpenGLTestLayer::OnEvent(Stimpi::BaseEvent* e)
{
	if (e->GetEventType() == Stimpi::EventType::WindowEvent)
	{
		auto* we = (Stimpi::WindowEvent*)e;
		if (we->GetType() == Stimpi::WindowEventType::WINDOW_EVENT_RESIZE)
		{
			//m_SceneCamera->Resize(we->GetWidth(), we->GetHeight());
		}
	}
}