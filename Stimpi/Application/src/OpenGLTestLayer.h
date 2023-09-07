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

	std::shared_ptr<Stimpi::Shader> m_Shader;
	std::shared_ptr<Stimpi::Camera> m_SceneCamera;
	std::shared_ptr<Stimpi::Texture> m_Texture;
	std::shared_ptr<Stimpi::Texture> m_Texture2;
};