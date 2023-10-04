#pragma once

#include <memory>

#include "Stimpi.h"

class OpenGLTestLayer : public Stimpi::Layer
{
public:
	OpenGLTestLayer();

	void OnAttach() override;
	void OnDetach() override;
	void Update(Stimpi::Timestep ts) override;
	void OnEvent(Stimpi::Event* e) override;

private:

	std::shared_ptr<Stimpi::Shader> m_Shader;
	std::shared_ptr<Stimpi::Shader> m_ShaderChecker;
	std::shared_ptr<Stimpi::Shader> m_ShaderSolidColor;
	std::shared_ptr<Stimpi::OrthoCamera> m_SceneCamera;
	std::shared_ptr<Stimpi::Texture> m_Texture;
	std::shared_ptr<Stimpi::Texture> m_Texture2;
	Stimpi::Texture* m_TextureRaw;

	std::shared_ptr<Stimpi::KeyboardEventHandler> m_KeyboardHandler;

	std::shared_ptr<Stimpi::Scene> m_Scene;
};