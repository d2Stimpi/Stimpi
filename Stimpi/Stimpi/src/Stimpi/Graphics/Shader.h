#pragma once

#include <glm/glm.hpp>

#include "Stimpi/Core/Core.h"

/*
* Shader shader(fileName):
* shader.Use();
* shader.SetUniform(name, value)
*  - name - string
*  - value - int, float, bool, glm::matX, glm::vecX 
*/

namespace Stimpi
{
	class ST_API Shader
	{
	public:
		virtual ~Shader();

		virtual void Use() = 0;
		virtual void SetUniform(const std::string& name, int value) = 0;
		virtual void SetUniform(const std::string& name, glm::vec3 value) = 0;
		virtual void SetUniform(const std::string& name, glm::vec4 value) = 0;
		virtual void SetUniform(const std::string& name, glm::mat4 value) = 0;

		virtual void CheckForErrors(unsigned int shader, std::string type) = 0;

		static Shader* CreateShader(const std::string& fileName);
	};
}