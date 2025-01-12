#include "stpch.h"
#include "Stimpi/Platform/OpenGLShader.h"

#include "Stimpi/Log.h"

#define DBG_LOG true

namespace Stimpi
{
	OpenGLShader::OpenGLShader(ShaderInfo info, VertexShaderData vsd, FragmentShaderData fsd)
		: Shader(info), m_ID(0)
	{
		// Compiling of shader code
		unsigned int vertexShader, fragmentShader;
		const char* cstrVertexShaderCode = vsd.c_str();
		const char* cstrFragmentShaderCode = fsd.c_str();
		// Vertex Shader
		vertexShader = glCreateShader(GL_VERTEX_SHADER);
		glShaderSource(vertexShader, 1, &cstrVertexShaderCode, NULL);
		glCompileShader(vertexShader);
		CheckForErrors(vertexShader, "Vertex");
		// Fragment Shader
		fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
		glShaderSource(fragmentShader, 1, &cstrFragmentShaderCode, NULL);
		glCompileShader(fragmentShader);
		CheckForErrors(fragmentShader, "Fragment");
		// Shader Program
		m_ID = glCreateProgram();
		glAttachShader(m_ID, vertexShader);
		glAttachShader(m_ID, fragmentShader);
		glLinkProgram(m_ID);
		CheckForErrors(m_ID, "Program");
		// Cleanup
		glDeleteShader(vertexShader);
		glDeleteShader(fragmentShader);

		if (DBG_LOG) LogShaderInfo();
	}

	OpenGLShader::~OpenGLShader()
	{
		glDeleteShader(m_ID);
	}

	unsigned int OpenGLShader::GetShaderID()
	{
		return m_ID;
	}

	void OpenGLShader::Use()
	{
		glUseProgram(m_ID);
	}

	bool OpenGLShader::Loaded()
	{
		return m_ID != 0;
	}

	void OpenGLShader::SetUniformImpl(const std::string& name, int value)
	{
		glUniform1i(glGetUniformLocation(m_ID, name.c_str()), value);
	}

	void OpenGLShader::SetUniformImpl(const std::string& name, float value)
	{
		glUniform1f(glGetUniformLocation(m_ID, name.c_str()), value);
	}

	void OpenGLShader::SetUniformImpl(const std::string& name, glm::vec2 value)
	{
		glUniform2fv(glGetUniformLocation(m_ID, name.c_str()), 1, &value[0]);
	}

	void OpenGLShader::SetUniformImpl(const std::string& name, glm::vec3 value)
	{
		glUniform3fv(glGetUniformLocation(m_ID, name.c_str()), 1, &value[0]);
	}

	void OpenGLShader::SetUniformImpl(const std::string& name, glm::vec4 value)
	{
		glUniform4fv(glGetUniformLocation(m_ID, name.c_str()), 1, &value[0]);
	}

	void OpenGLShader::SetUniformImpl(const std::string& name, glm::mat4 value)
	{
		glUniformMatrix4fv(glGetUniformLocation(m_ID, name.c_str()), 1, GL_FALSE, &value[0][0]);
	}

	void OpenGLShader::CheckForErrors(unsigned int shader, std::string type)
	{
		int success;
		char infoLog[1024];
		if (type != "PROGRAM")
		{
			glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
			if (!success)
			{
				glGetShaderInfoLog(shader, 1024, NULL, infoLog);
				ST_CORE_ERROR("{0} shader compilation error: {1}", type, infoLog);
			}
		}
		else
		{
			glGetProgramiv(shader, GL_LINK_STATUS, &success);
			if (!success)
			{
				glGetProgramInfoLog(shader, 1024, NULL, infoLog);
				ST_CORE_ERROR("{0} shader linking error: {1}", type, infoLog);
			}
		}
	}
}