#include "OpenGLTestLayer.h"


static std::shared_ptr<Stimpi::Shader> s_bgShader;
static std::shared_ptr<Stimpi::Shader> s_testureShader;
static std::shared_ptr<Stimpi::Camera> s_bgCamera;
static Stimpi::Texture* s_Texture;

OpenGLTestLayer::OpenGLTestLayer() 
	: Layer("OpenGL TestLayer") 
{
	// Temp, figure out a better place for this kind of "default scene" stuff
	s_bgCamera = std::make_shared<Stimpi::Camera>(0.0f, 1280.0f, 0.0f, 720.0f);
	s_bgCamera->SetPosition({ 0.0f, 0.0f, 0.0f });

	s_bgShader.reset(Stimpi::Shader::CreateShader("..\/assets\/shaders\/checkerboard.shader"));
	s_testureShader.reset(Stimpi::Shader::CreateShader("..\/assets\/shaders\/shader.shader"));
	s_Texture = Stimpi::ResourceManager::Instance()->LoadTexture("..\/assets\/textures\/Capture.JPG");
}

void OpenGLTestLayer::OnAttach() 
{
	ST_TRACE("{0}: OnAttach", m_DebugName); 
	Stimpi::Renderer2D::Instance()->EnableLocalRendering(true);
}

void OpenGLTestLayer::OnDetach()
{ 
	ST_TRACE("{0}: OnDetach", m_DebugName); 
}

void OpenGLTestLayer::Update(Stimpi::Timestep ts)
{
	// Custom Rendering stuff
	auto canvasWidth = Stimpi::Renderer2D::Instance()->GetCanvasWidth();
	auto canvasHeight = Stimpi::Renderer2D::Instance()->GetCanvasHeight();

	//ST_INFO("FB size: {0}, {1}", canvasWidth, canvasHeight);

	s_bgShader->SetUniform("u_Projection", s_bgCamera->GetProjectionMatrix());
	s_bgShader->SetUniform("u_resolution", glm::vec2(canvasWidth, canvasHeight));

	s_testureShader->SetUniform("u_Projection", s_bgCamera->GetProjectionMatrix());

	Stimpi::Renderer2D::Instance()->BeginScene(s_bgCamera->GetOrthoCamera());
	Stimpi::Renderer2D::Instance()->Submit({ 0.0f, 0.0f, canvasWidth, canvasHeight }, s_bgShader.get());
	Stimpi::Renderer2D::Instance()->Submit({ 0.0f, 0.0f, s_bgCamera->GetViewportWidth(), s_bgCamera->GetViewportHeight() }, s_Texture, s_testureShader.get());
	Stimpi::Renderer2D::Instance()->EndScene();

	Stimpi::Renderer2D::Instance()->StartFrame();
	Stimpi::Renderer2D::Instance()->DrawFrame();
	Stimpi::Renderer2D::Instance()->EndFrame();
}

void OpenGLTestLayer::OnEvent(Stimpi::Event* e)
{
	Stimpi::EventDispatcher<Stimpi::WindowEvent> dispatcher;
	dispatcher.Dispatch(e, [&](Stimpi::WindowEvent* e) -> bool {
		if (e->GetType() == Stimpi::WindowEventType::WINDOW_EVENT_RESIZE)
		{
			s_bgCamera->Resize(0.0f, e->GetWidth(), 0.0f, e->GetHeight());
		}
		return false;
		});
}