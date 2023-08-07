#include "Layer.h"

#include "../Log.h"

namespace Stimpi
{
	LayerStack::~LayerStack()
	{
		for (auto layer : m_Layers)
			delete layer;
	}

	void LayerStack::AttachLayer(Layer* layer)
	{
		if (layer != nullptr)
		{
			if (std::find(m_Layers.begin(), m_Layers.end(), layer) == m_Layers.end())
			{
				m_Layers.emplace(m_Layers.begin() + m_LayersCount, layer);
				m_LayersCount++;
				layer->OnAttach();
			}
			else
			{
				ST_CORE_WARN("Layer already added!");
			}
		}
	}

	void LayerStack::AttachOverlay(Layer* layer)
	{
		if (layer != nullptr)
		{
			if (std::find(m_Layers.begin(), m_Layers.end(), layer) == m_Layers.end())
			{
				m_Layers.emplace_back(layer);
				layer->OnAttach();
			}
			else
			{
				ST_CORE_WARN("Overlay Layer already added!");
			}
		}
	}

	void LayerStack::DetachLayer(Layer* layer)
	{
		
		if (std::find(m_Layers.begin(), m_Layers.end(), layer) != m_Layers.end())
		{
			auto iter = std::find(m_Layers.begin(), m_Layers.end(), layer);
			uint8_t position = iter - m_Layers.begin();

			if (m_LayersCount > position)
				m_LayersCount--;

			m_Layers.erase(std::remove(m_Layers.begin(), m_Layers.end(), layer));
			layer->OnDetach();
		}
	}

	void LayerStack::Update()
	{
		for (auto layer : m_Layers)
		{
			layer->Update();
		}
	}

	void LayerStack::Render()
	{
		for (auto layer : m_Layers)
		{
			layer->Render();
		}
	}
}