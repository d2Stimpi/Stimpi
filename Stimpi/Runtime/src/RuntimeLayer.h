#pragma once

#include "Stimpi/Core/Layer.h"
#include "Stimpi/Scene/Scene.h"

namespace Stimpi
{
	class RuntimeLayer : public Layer
	{
	public:
		RuntimeLayer();
		~RuntimeLayer();

		void OnAttach() override;
		void OnDetach() override;
		void Update(Timestep ts) override;
		void OnEvent(Event* e) override;

	private:
		std::shared_ptr<Scene> m_Scene;

		//Temp
		std::shared_ptr<Camera> m_EditorCamera;
		std::shared_ptr<CameraController> m_CameraController;
	};
}