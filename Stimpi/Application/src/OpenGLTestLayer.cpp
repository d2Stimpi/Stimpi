#include "OpenGLTestLayer.h"


OpenGLTestLayer::OpenGLTestLayer() 
	: Layer("OpenGL TestLayer") 
{
	m_Shader.reset(Stimpi::Shader::CreateShader("shaders\/shader.shader"));
	m_ShaderChecker.reset(Stimpi::Shader::CreateShader("shaders\/checkerboard.shader"));
	m_SceneCamera = std::make_shared<Stimpi::Camera>(1280.0f, 720.0f);
	m_Texture.reset(Stimpi::Texture::CreateTexture("Capture.jpg"));
	//m_TextureRaw = Stimpi::ResourceManager::Instance()->LoadTexture("Picture1.jpg");
	m_Texture2.reset(Stimpi::Texture::CreateTexture("Picture1.jpg"));

	// Just for test, no need to use here since Layer::OnEvent exists
	m_KeyboardHandler = std::make_shared<Stimpi::KeyboardEventHandler>([](Stimpi::KeyboardEvent event) -> bool {
			ST_INFO("Handling KeyboardEvent key: {0}", event.GetKeyCode());
			return false;
		});
	//Register handler
	Stimpi::InputManager::Instance()->AddKeyboardEventHandler(m_KeyboardHandler.get());

	// TODO: temp test stuff
	auto resourceManager = Stimpi::ResourceManager::Instance();
	resourceManager->ClearFile("TestFile.data");
	//resourceManager->WriteToFile("TestFile.data", "This is a test string");
	resourceManager->Test();

	//m_Scene = std::make_shared<Stimpi::Scene>();
	m_Scene = std::make_shared<Stimpi::Scene>("SceneTest.data");
	m_Scene->LoadSnece("SceneTest.data");
	//m_Scene->SaveScene("SceneTest.data");

	//Set as active Scene
	Stimpi::SceneManager::Instance()->SetActiveScene(m_Scene.get());
}

void OpenGLTestLayer::OnAttach() 
{
	ST_TRACE("{0}: OnAttach", m_DebugName); 
}

void OpenGLTestLayer::OnDetach()
{ 
	ST_TRACE("{0}: OnDetach", m_DebugName); 
}

void OpenGLTestLayer::Update(Stimpi::Timestep ts)
{
	// Match camera display area to framebuffer size
	auto canvasWidth = Stimpi::Renderer2D::Instace()->GetCanvasWidth();
	auto canvasHeight = Stimpi::Renderer2D::Instace()->GetCanvasHeight();
	static float moveSpeed = 400.f;
	static float zoomSpeed = 10.0f;
	static float zoomFactor = 1.0f;

	m_SceneCamera->Resize(canvasWidth, canvasHeight);

	m_Shader->SetUniform("u_proj", m_SceneCamera->GetProjectionMatrix());
	m_Shader->SetUniform("u_model", m_SceneCamera->GetModelMatrix());
	m_Shader->SetUniform("u_texture", 0);
	
	m_ShaderChecker->SetUniform("u_proj", m_SceneCamera->GetProjectionMatrix());
	m_ShaderChecker->SetUniform("u_model", m_SceneCamera->GetModelMatrix());
	m_ShaderChecker->SetUniform("u_resolution", glm::vec2(canvasWidth, canvasHeight));

	// Movement control
	if (Stimpi::InputManager::IsKeyPressed(ST_KEY_I))
		m_SceneCamera->Translate({0.0f, moveSpeed * ts, 0.0f});
	if (Stimpi::InputManager::IsKeyPressed(ST_KEY_K))
		m_SceneCamera->Translate({ 0.0f, -moveSpeed * ts, 0.0f });
	if (Stimpi::InputManager::IsKeyPressed(ST_KEY_L))
		m_SceneCamera->Translate({ moveSpeed * ts, 0.0f, 0.0f });
	if (Stimpi::InputManager::IsKeyPressed(ST_KEY_J))
		m_SceneCamera->Translate({ -moveSpeed * ts, 0.0f, 0.0f });
	// Zoom control
	if (Stimpi::InputManager::IsKeyPressed(ST_KEY_U))
	{
		zoomFactor += zoomSpeed * ts;
		if (zoomFactor >= 10.0f) zoomFactor = 10.0f;
		ST_INFO("Zoom: {0}", zoomFactor);
	}
	if (Stimpi::InputManager::IsKeyPressed(ST_KEY_O))
	{
		zoomFactor -= zoomSpeed * ts;
		if (zoomFactor <= 0.0f) zoomFactor = 0.001f;
		ST_INFO("Zoom: {0}", zoomFactor);
	}
	//m_SceneCamera->Zoom(zoomFactor);
	
	Stimpi::Renderer2D::Instace()->BeginScene(m_SceneCamera.get(), m_ShaderChecker.get());
	{
		Stimpi::Renderer2D::Instace()->PushQuad(0.0f, 0.0f, canvasWidth, canvasHeight, 1.0f, 1.0f);
	}
	Stimpi::Renderer2D::Instace()->EndScene();

	Stimpi::Renderer2D::Instace()->BeginScene(m_SceneCamera.get(), m_Shader.get());
	{
		Stimpi::Renderer2D::Instace()->UseTexture(m_Texture.get());
		Stimpi::Renderer2D::Instace()->PushQuad(200.0f, 100.0f, canvasWidth/2, canvasHeight/2, 1.0f, 1.0f);
		//Stimpi::Renderer2D::Instace()->UseTexture(m_Texture.get());
		//Stimpi::Renderer2D::Instace()->PushQuad(0.0f, 0.0f, 250.f, 200.0f, 1.0f, 1.0f);
		//Stimpi::Renderer2D::Instace()->UseTexture(m_Texture2.get());
		//Stimpi::Renderer2D::Instace()->PushQuad(550.0f, 550.0f, 250.f, 200.0f, 1.0f, 1.0f);
		//Stimpi::Renderer2D::Instace()->UseTexture(m_Texture.get());
		//Stimpi::Renderer2D::Instace()->PushQuad(750.0f, 850.0f, 250.f, 200.0f, 1.0f, 1.0f);
	}
	Stimpi::Renderer2D::Instace()->EndScene();

	//Setup scene here for now
	Stimpi::Renderer2D::Instace()->BeginScene(m_SceneCamera.get(), m_Shader.get());
	{
		Stimpi::Renderer2D::Instace()->UseTexture(m_Texture.get());
		//Update test scene
		m_Scene->OnUpdate(ts);
	}
	Stimpi::Renderer2D::Instace()->EndScene();
}

void OpenGLTestLayer::OnEvent(Stimpi::Event* e)
{
	//ST_CORE_INFO("OnEvent - OpenGLTestLayer");
	if (e->GetEventType() == Stimpi::EventType::WindowEvent)
	{
		auto* we = (Stimpi::WindowEvent*)e;
		if (we->GetType() == Stimpi::WindowEventType::WINDOW_EVENT_RESIZE)
		{
			//m_SceneCamera->Resize(we->GetWidth(), we->GetHeight());
		}
	}
}