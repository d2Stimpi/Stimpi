#pragma once

#include "stpch.h"
#include "Stimpi/Core/Core.h"

#include <type_traits>
#include <unordered_map>

#include <glm/glm.hpp>

/*
* Shader shader(fileName):
* shader.Use();
* shader.SetUniform(name, value)
*  - name - string
*  - value - int, float, bool, glm::matX, glm::vecX 
*/

namespace Stimpi
{
	using shader_variant = std::variant<int, float, glm::vec2, glm::vec3, glm::vec4, glm::mat4>;

	class ST_API Shader
	{
	public:
		virtual ~Shader();

		virtual unsigned int GetShaderID() = 0;

		// Also pass all "buffered" uniform data to shader program
		virtual void Use() = 0;

		void SetUniform(const std::string name, shader_variant value);
		void SetBufferedUniforms();
		void ClearBufferedUniforms();

		static Shader* CreateShader(const std::string& fileName);

		// AssetManager
		static Shader* Create(std::string& file) { return CreateShader(file); }
		virtual bool Loaded() = 0;
	private:
		virtual void SetUniformImpl(const std::string& name, int value) = 0;
		virtual void SetUniformImpl(const std::string& name, float value) = 0;
		virtual void SetUniformImpl(const std::string& name, glm::vec2 value) = 0;
		virtual void SetUniformImpl(const std::string& name, glm::vec3 value) = 0;
		virtual void SetUniformImpl(const std::string& name, glm::vec4 value) = 0;
		virtual void SetUniformImpl(const std::string& name, glm::mat4 value) = 0;

		virtual void CheckForErrors(unsigned int shader, std::string type) = 0;

		// name : value for buffering Uniforms
		std::unordered_map<std::string, shader_variant> m_UniformList;
	};
}