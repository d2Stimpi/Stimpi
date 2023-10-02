#pragma once

#include <memory>
#include <vector>
#include <string>

#include "Stimpi/Core/Core.h"
#include "Stimpi/Core/Event.h"
#include "Stimpi/Core/Timestep.h"

namespace Stimpi
{
	class ST_API Layer
	{
	public:
		Layer(const std::string& name = "Layer") : m_DebugName(name) {};
		virtual ~Layer() {};

		virtual void OnAttach() = 0;
		virtual void OnDetach() = 0;
		virtual void Update(Timestep ts) = 0;
		virtual void OnEvent(Event* e) = 0;

		inline const std::string& GetName() { return m_DebugName; }
	protected:
		std::string m_DebugName;
	};

	/* Layers - gives a way to distinguish and control blocks(layers) of processing engin logic and consuming events
	*  - UI
	*  - Scene rendering
	*  - overlay rendering etc...
	* 
	* Process order:
	*  1) Layers
	*  2) Overlay Layers
	*/
	class ST_API LayerStack
	{
	public:
		~LayerStack();

		void AttachLayer(Layer* layer);
		void AttachOverlay(Layer* layer);
		void DetachLayer(Layer* layer);

		void Update(Timestep ts);
		void OnEvent(Event* e);

		std::vector<Layer*>::iterator begin() { return m_Layers.begin(); }
		std::vector<Layer*>::iterator end() { return m_Layers.end(); }
	private:
		uint8_t m_LayersCount{ 0 }; // Used to track non-overlay layers in the m_Layers vector
		std::vector<Layer*> m_Layers;
	};
}