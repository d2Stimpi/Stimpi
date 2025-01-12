#pragma once

#include "Stimpi/Graphics/Shader.h"

#include <glad/glad.h>
#include <glm/gtc/type_ptr.hpp>

namespace Stimpi
{
	class OpenGLShader : public Shader
	{
	public:
		OpenGLShader(ShaderInfo info, VertexShaderData vsd, FragmentShaderData fsd);
		~OpenGLShader();

		unsigned int GetShaderID() override;
		void Use() override;
		bool Loaded() override;
	
	private:
		void SetUniformImpl(const std::string& name, int value) override;
		void SetUniformImpl(const std::string& name, float value) override;
		void SetUniformImpl(const std::string& name, glm::vec2 value) override;
		void SetUniformImpl(const std::string& name, glm::vec3 value) override;
		void SetUniformImpl(const std::string& name, glm::vec4 value) override;
		void SetUniformImpl(const std::string& name, glm::mat4 value) override;

		void CheckForErrors(unsigned int shader, std::string type) override;

	private:
		unsigned int m_ID = 0;
	};
}