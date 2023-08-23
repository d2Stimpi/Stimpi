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
		Shader(const std::string& fileName);
		~Shader();

		void Use();
		void SetUniform(const std::string& name, int value);
		void SetUniform(const std::string& name, glm::vec3 value);
		void SetUniform(const std::string& name, glm::mat4 value);

	private:
		void CheckForErrors(unsigned int shader, std::string type);

		unsigned int m_ID;
	};
}