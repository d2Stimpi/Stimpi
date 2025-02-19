#pragma once

#include "ImGui/src/imgui.h"

#include <yaml-cpp/yaml.h>

YAML::Emitter& operator<<(YAML::Emitter& out, const ImVec2& vec);

namespace YAML {
	template<>
	struct convert<ImVec2> {
		static Node encode(const ImVec2& rhs) {
			Node node;
			node.push_back(rhs.x);
			node.push_back(rhs.y);
			return node;
		}

		static bool decode(const Node& node, ImVec2& rhs) {
			if (!node.IsSequence() || node.size() != 2) {
				return false;
			}

			rhs.x = node[0].as<float>();
			rhs.y = node[1].as<float>();
			return true;
		}
	};
}