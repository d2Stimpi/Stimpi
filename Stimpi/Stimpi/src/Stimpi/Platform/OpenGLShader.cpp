#include "Stimpi/Platform/OpenGLShader.h"

#include <iostream>
#include <glad/glad.h>
#include <glm/gtc/type_ptr.hpp>

#include "Stimpi/Log.h"

namespace Stimpi
{
	OpenGLShader::OpenGLShader(const std::string& fileName)
	{
		std::string vertexShaderCode;
		std::string fragmentShaderCode;
		std::ifstream shaderFile;

		shaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);

		try
		{
			std::stringstream vertexStream;
			std::stringstream fragmentStream;
			std::string line;
			bool readingFragmentShader = false;

			shaderFile.open(fileName);

			while (!shaderFile.eof())
			{
				std::getline(shaderFile, line);
				if (line.compare("#fragment") == 0)
				{
					readingFragmentShader = true;
					continue;
				}
				if (!readingFragmentShader)
				{
					vertexStream << line << std::endl;
					std::cout << line << std::endl;
				}
				else
				{
					fragmentStream << line << std::endl;
					std::cout << line << std::endl;
				}
			}
			vertexShaderCode = vertexStream.str();
			fragmentShaderCode = fragmentStream.str();

			shaderFile.close();
		}
		catch (std::ifstream::failure& e)
		{
			ST_CORE_ERROR("Shader: failed to read shader file {0} - error: {1}\n", fileName, e.what());
		}

		// Compiling of shader code
		unsigned int vertexShader, fragmentShader;
		const char* cstrVertexShaderCode = vertexShaderCode.c_str();
		const char* cstrFragmentShaderCode = fragmentShaderCode.c_str();
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

	void OpenGLShader::SetUniform(const std::string& name, int value)
	{
		glUniform1i(glGetUniformLocation(m_ID, name.c_str()), value);
	}

	void OpenGLShader::SetUniform(const std::string& name, glm::vec3 value)
	{
		glUniform3fv(glGetUniformLocation(m_ID, name.c_str()), 1, &value[0]);
	}

	void OpenGLShader::SetUniform(const std::string& name, glm::vec4 value)
	{
		glUniform3fv(glGetUniformLocation(m_ID, name.c_str()), 1, &value[0]);
	}

	void OpenGLShader::SetUniform(const std::string& name, glm::mat4 value)
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
				ST_CORE_ERROR("{0} shader copilation error: {1}", type, infoLog);
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