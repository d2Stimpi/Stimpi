#include "stpch.h"
#include "Gui/Nodes/GraphController.h"

#include "Gui/EditorUtils.h"
#include "Gui/Nodes/GraphPanel.h"
#include "Stimpi/Core/InputManager.h"

#define CANVAS_MOVE_BUTTON ImGuiMouseButton_Left

namespace Stimpi
{
	GraphContorller::GraphContorller(GraphPanel* panelContext)
	{
		m_PanelContext = panelContext;
	}

	GraphContorller::~GraphContorller()
	{

	}

	void GraphContorller::SetDrawCanvas(GraphPanelCanvas* canvas)
	{
		m_Canvas = canvas;
	}

	void GraphContorller::SetActiveGraph(Graph* graph)
	{
		m_Graph = graph;
	}

	void GraphContorller::UpdateMouseControls()
	{
		ImGuiIO& io = ImGui::GetIO();

		if (!EditorUtils::IsMouseContainedInRegion(m_Canvas->m_PosMin, m_Canvas->m_PosMax))
			return;

		uint32_t hoverNodeID = m_PanelContext->GetMouseHoverNode();	// Reset selection later, if mouse action is finished and nothing is hovered over
		static ImVec2 dragOffset(0.0f, 0.0f);
		bool  showPopup = false;

		if (io.MouseWheel != 0)
		{
			if (io.MouseWheel > 0)
			{
				m_PanelContext->SetPanelZoom(m_PanelContext->GetPanelZoom() + 0.1f);
			}
			else
			{
				m_PanelContext->SetPanelZoom(m_PanelContext->GetPanelZoom() - 0.1f);
			}
		}

		switch (m_Action)
		{
		case ControllAction::NONE:
			if (hoverNodeID != 0)
			{
				m_Action = ControllAction::NODE_HOVER;
				ImGui::SetMouseCursor(ImGuiMouseCursor_ResizeAll);
			}
			else
			{
				// Clear node selection if clicked on empty space
				if (ImGui::IsMouseClicked(ImGuiMouseButton_Left))
				{
					m_SelectedNode = nullptr;
					m_PanelContext->OnNodeDeselect();

					// Check for hovered connection lines and select one if found
					// Done on mouse click to avoid checking each frame for line hovers
					m_SelectedConnection = m_PanelContext->GetMouseHoveredConnection();
					if (m_SelectedConnection)
					{
						m_Action = ControllAction::CONNECTION_ONPRESS;
						break;
					}
				}

				// Nothing hovered, start canvas move action on click
				if (ImGui::IsMouseClicked(CANVAS_MOVE_BUTTON))
				{
					m_Action = ControllAction::CANVAS_MOVE;
					break;
				}

				// Nothing hovered so we can show add new node pop up
				if (ImGui::IsMouseClicked(ImGuiMouseButton_Right))
				{
					m_Action = ControllAction::SHOW_POPUP_ONRELEASE;
					break;
				}
			}

			break;
		case ControllAction::NODE_HOVER:
			if (hoverNodeID == 0)
			{
				m_Action = ControllAction::NONE;
				break;
			}
			else
			{
				ImGui::SetMouseCursor(ImGuiMouseCursor_ResizeAll);

				// Check if we are hovering any Pins
				Node* hoveredNode = m_PanelContext->GetNodeByID(hoverNodeID);
				if (hoveredNode)
				{
					Pin* hoveredPin = m_PanelContext->GetMouseHoveredPin(hoveredNode);
					if (hoveredPin)
					{

						ImGui::SetMouseCursor(ImGuiMouseCursor_Arrow);

						if (ImGui::IsMouseClicked(ImGuiMouseButton_Left))
						{
							//ST_CORE_INFO("Pin {} clicked", hoveredPin->m_ID);
							// Clear Connection hover selection
							m_SelectedConnection = nullptr;

							m_Action = ControllAction::NODE_PIN_DRAG;
							m_SelectedPin = hoveredPin;
							m_PinFloatingTarget = m_PanelContext->GetNodePanelViewClickLocation();
							break; //exit case
						}
					}
				}
			}

			if (ImGui::IsMouseClicked(ImGuiMouseButton_Left))
			{
				m_Action = ControllAction::NODE_DRAGABLE;
				m_SelectedNode = m_PanelContext->GetNodeByID(hoverNodeID);

				// Clear Connection hover selection
				m_SelectedConnection = nullptr;
			}
			break;
		case ControllAction::NODE_DRAGABLE:
			ImGui::SetMouseCursor(ImGuiMouseCursor_ResizeAll);

			if (ImGui::IsMouseReleased(ImGuiMouseButton_Left))
			{
				// Node Dragging finished - find all connections to the moved node and update points for mouse picking
				m_PanelContext->UpdateNodeConnectionsPoints(m_SelectedNode);

				m_Action = ControllAction::NONE;
				break;
			}
			else if (ImGui::IsMouseDragging(ImGuiMouseButton_Left))
			{
				ImGui::SetMouseCursor(ImGuiMouseCursor_ResizeAll);

				dragOffset.x = io.MouseDelta.x / m_Canvas->m_Scale;
				dragOffset.y = io.MouseDelta.y / m_Canvas->m_Scale;

				Node* selected = m_SelectedNode;
				if (selected != nullptr)
				{
					selected->m_Pos.x += dragOffset.x;
					selected->m_Pos.y += dragOffset.y;
				}
			}
			else
			{
				dragOffset.x = 0.0f;
				dragOffset.y = 0.0f;
			}
			break;
		case ControllAction::NODE_PIN_DRAG:
			if (ImGui::IsMouseReleased(ImGuiMouseButton_Left))
			{
				// clear drag target
				m_Action = ControllAction::NONE;

				// Handle pin drag-drop
				if (hoverNodeID != 0)
				{
					Node* hoveredNode = m_PanelContext->GetNodeByID(hoverNodeID);
					if (hoveredNode != nullptr && hoveredNode != m_SelectedPin->m_ParentNode)
					{
						Pin* target = m_PanelContext->GetMouseHoveredPin(hoveredNode);
						if (target)
						{
							//ST_CORE_INFO("Target Node::Pin {} - {} selected", hoveredNode->m_ID, target->m_ID);
							// Handle forming Pin - Pin connection
							ConnectPinToPin(m_SelectedPin, target, m_Graph);
						}
					}
				}

				m_SelectedPin = nullptr;
				break;
			}
			else  // Mouse button still pressed - node pin dragging
			{
				// Update floating target
				m_PinFloatingTarget = m_PanelContext->GetNodePanelViewClickLocation();
			}
			break;
		case ControllAction::CONNECTION_ONPRESS:
			if (ImGui::IsMouseReleased(ImGuiMouseButton_Left))
			{
				//ST_CORE_INFO("Hovering stopped - back to none");
				m_Action = ControllAction::NONE;
				break;
			}
			else
			{
				ImVec2 mouseDragDelta = ImGui::GetMouseDragDelta();
				if (mouseDragDelta.x != 0 || mouseDragDelta.y != 0)
				{
					//ST_CORE_INFO("Dragging connection - mouse delta {}, {}", mouseDragDelta.x, mouseDragDelta.y);
					PinConnection* connection = m_SelectedConnection;
					if (connection)
					{
						m_Action = ControllAction::NODE_PIN_DRAG;
						m_SelectedPin = connection->m_SourcePin;
						m_PinFloatingTarget = m_PanelContext->GetNodePanelViewClickLocation();

						BreakPinToPinConnection(connection, m_Graph);
						m_SelectedConnection = nullptr;
						break;
					}
				}
			}

			break;
		case ControllAction::SHOW_POPUP_ONRELEASE:
			if (ImGui::IsMouseReleased(ImGuiMouseButton_Right))
			{
				m_PanelContext->SetZoomEnable(false);
				showPopup = true;
				m_Action = ControllAction::POPUP_ACTIVE;
			}
			break;
		case ControllAction::POPUP_ACTIVE:
			if (ImGui::IsMouseClicked(ImGuiMouseButton_Right) || ImGui::IsMouseClicked(ImGuiMouseButton_Left))
			{
				m_Action = ControllAction::NONE;
				m_PanelContext->SetZoomEnable(true);
			}
			break;
		case ControllAction::CANVAS_MOVE:
			if (m_IsActive && ImGui::IsMouseDragging(CANVAS_MOVE_BUTTON))
			{
				m_Canvas->m_Scrolling.x += io.MouseDelta.x;
				m_Canvas->m_Scrolling.y += io.MouseDelta.y;
				// updated origin when changing scroll offset, avoids 1 frame update delay
				m_Canvas->m_Origin = { m_Canvas->m_PosMin.x + m_Canvas->m_Scrolling.x, m_Canvas->m_PosMin.y + m_Canvas->m_Scrolling.y };
			}


			if (ImGui::IsMouseReleased(CANVAS_MOVE_BUTTON))
			{
				m_Action = ControllAction::NONE;
				break;
			}
			break;
		default:
			break;
		}

		m_PanelContext->AddNodePopup(showPopup);
	}

	void GraphContorller::HandleKeyPresses()
	{
		if (InputManager::Instance()->IsKeyPressed(ST_KEY_DELETE))
		{
			if (m_SelectedNode)
			{
				// Remove Node
				m_PanelContext->OnNodeDeleted(m_SelectedNode);
				m_SelectedNode = nullptr;
			}

			if (m_SelectedConnection)
			{
				BreakPinToPinConnection(m_SelectedConnection, m_Graph);
				m_SelectedConnection = nullptr;
			}
		}
	}

	void GraphContorller::SetSelectedNode(Node* node)
	{
		m_SelectedNode = node;
	}

	Stimpi::Node* GraphContorller::GetSelectedNode()
	{
		return m_SelectedNode;
	}

	Stimpi::Pin* GraphContorller::GetSelectedPin()
	{
		return m_SelectedPin;
	}

	Stimpi::ControllAction GraphContorller::GetAction()
	{
		return m_Action;
	}

	Stimpi::PinConnection* GraphContorller::GetSeletedPinConnection()
	{
		return m_SelectedConnection;
	}

	ImVec2& GraphContorller::GetPinFloatingTarget()
	{
		return m_PinFloatingTarget;
	}

	void GraphContorller::UpdateInternals()
	{

	}

}