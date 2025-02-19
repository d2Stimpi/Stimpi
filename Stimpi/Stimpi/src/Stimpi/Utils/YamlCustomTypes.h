#pragma once

#include "Stimpi/Core/Core.h"

#include <yaml-cpp/yaml.h>
#include <glm/glm.hpp>

// glm types

ST_API YAML::Emitter& operator<<(YAML::Emitter& out, const glm::vec2& vec);