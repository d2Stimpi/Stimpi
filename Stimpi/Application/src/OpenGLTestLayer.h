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
};