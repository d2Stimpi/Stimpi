#pragma once

#include "Stimpi/Graphics/Shader.h"

namespace Stimpi
{
	class OpenGLShader : public Shader
	{
	public:
		OpenGLShader(const std::string& fileName);
		~OpenGLShader();

		unsigned int GetShaderID() override;

		void Use() override;
		void SetUniform(const std::string& name, int value) override;
		void SetUniform(const std::string& name, glm::vec3 value) override;
		void SetUniform(const std::string& name, glm::vec4 value) override;
		void SetUniform(const std::string& name, glm::mat4 value) override;
		void CheckForErrors(unsigned int shader, std::string type) override;

	private:
		unsigned int m_ID;
	};
}