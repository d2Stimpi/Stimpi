#pragma once

#include "Stimpi/Graphics/Shader.h"

#include <glad/glad.h>
#include <glm/gtc/type_ptr.hpp>

namespace Stimpi
{
	class OpenGLShader : public Shader
	{
	public:
		OpenGLShader(const std::string& fileName);
		~OpenGLShader();

		unsigned int GetShaderID() override;
		void Use() override;

		virtual bool Loaded() override;
	
	private:
		void SetUniformImpl(const std::string& name, int value) override;
		void SetUniformImpl(const std::string& name, float value) override;
		void SetUniformImpl(const std::string& name, glm::vec2 value) override;
		void SetUniformImpl(const std::string& name, glm::vec3 value) override;
		void SetUniformImpl(const std::string& name, glm::vec4 value) override;
		void SetUniformImpl(const std::string& name, glm::mat4 value) override;

		void CheckForErrors(unsigned int shader, std::string type) override;

		void ParseVertexShaderLine(const std::string& line);
		/* Data members */
		unsigned int m_ID;
	};
}