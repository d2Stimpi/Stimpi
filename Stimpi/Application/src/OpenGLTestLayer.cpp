#include "OpenGLTestLayer.h"

OpenGLTestLayer::OpenGLTestLayer() 
	: Layer("OpenGL TestLayer") 
{
	/*m_Shader = std::make_shared<Stimpi::Shader>("shader.shader");

	m_VAO.reset(Stimpi::VertexArrayObject::CreateVertexArrayObject());
	m_VBO.reset(Stimpi::VertexBufferObject::CreateVertexBufferObject(Stimpi::DrawType::STATIC_DRAW));
	m_EBO.reset(Stimpi::ElementBufferObject::CreateElementBufferObject(Stimpi::DrawType::STATIC_DRAW));

	m_VAO->Bind();
	m_VBO->SetData(m_Vertices, sizeof(m_Vertices));
	m_EBO->SetData(m_Indices, sizeof(m_Indices));
	m_VAO->SetData({3, 3, 2});*/
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
	/*m_Shader->Use();
	m_VAO->Bind();
	Stimpi::Renderer2D::DrawElements(sizeof(m_Indices)/sizeof(float));*/
}

void OpenGLTestLayer::OnEvent(Stimpi::BaseEvent* e)
{

}