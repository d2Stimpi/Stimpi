#include "OpenGLTestLayer.h"


OpenGLTestLayer::OpenGLTestLayer() 
	: Layer("OpenGL TestLayer") 
{
	// TODO: temp test stuff
	auto resourceManager = Stimpi::ResourceManager::Instance();
	resourceManager->ClearFile("TestFile.data");
	//resourceManager->WriteToFile("TestFile.data", "This is a test string");
	resourceManager->Test();
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

}

void OpenGLTestLayer::OnEvent(Stimpi::Event* e)
{
	
}