#pragma once

#include <memory>

#include "Stimpi.h"

class OpenGLTestLayer : public Stimpi::Layer
{
public:
	OpenGLTestLayer();

	void OnAttach() override;
	void OnDetach() override;
	void Update() override;
	void OnEvent(Stimpi::BaseEvent* e) override;

private:
	float m_Vertices[32] = {
		// positions          // colors           // texture coords
		 1.0f,  1.0f, 0.0f,   1.0f, 0.0f, 0.0f,   1.0f, 1.0f,   // top right
		 1.0f,  0.0f, 0.0f,   0.0f, 1.0f, 0.0f,   1.0f, 0.0f,   // bottom right
		 0.0f,  0.0f, 0.0f,   0.0f, 0.0f, 1.0f,   0.0f, 0.0f,   // bottom left
		 0.0f,  1.0f, 0.0f,   1.0f, 1.0f, 0.0f,   0.0f, 1.0f    // top left 
	};
	unsigned int m_Indices[6] = {  // note that we start from 0!
		0, 1, 3,   // first triangle
		1, 2, 3    // second triangle
	};

	/*std::shared_ptr<Stimpi::Shader> m_Shader;
	std::shared_ptr<Stimpi::VertexArrayObject> m_VAO;
	std::shared_ptr<Stimpi::VertexBufferObject> m_VBO;
	std::shared_ptr<Stimpi::ElementBufferObject> m_EBO;*/
};