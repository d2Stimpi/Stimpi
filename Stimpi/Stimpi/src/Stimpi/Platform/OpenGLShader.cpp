#include "stpch.h"
#include "Stimpi/Platform/OpenGLShader.h"

#include "Stimpi/Log.h"

#define DBG_LOG true

namespace Stimpi
{
	OpenGLShader::OpenGLShader(const std::string& fileName)
		: Shader(fileName)
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
					ParseVertexShaderLine(line);
					vertexStream << line << std::endl;
				}
				else
				{
					fragmentStream << line << std::endl;
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

	void OpenGLShader::ParseVertexShaderLine(const std::string& line)
	{
		size_t pos = line.find("layout(location =");
		if (pos != std::string::npos)
		{
			// find the location number, we will ignore everything before num. 3 ??
			int location = std::stoi(line.substr(17, line.length() - 17));
			//if (location > 2)
			//{
				// find ")" and get tokens after it
				pos = line.find(")", pos + 1);
				if (pos != std::string::npos)
				{

					std::string substr = line.substr(pos + 1, line.length() - pos - 1);
					const char* delimiter = " ";
					char* token = std::strtok(substr.data(), delimiter);
				
					std::string modifier = "";
					std::string type = "";
					std::string name = "";

					if (token)
					{
						modifier = std::string(token);
						token = std::strtok(nullptr, delimiter);
					}

					if (token)
					{
						type = std::string(token);
						token = std::strtok(nullptr, delimiter);
					}
				
					if (token)
					{
						name = std::string(token);
						name.pop_back();	// remove ';' char
					}

					ShaderInfo& info = GetInfo();
					info.m_ShaderLayout.m_Data.emplace_back(StringToShaderType(type), name);
					//ST_CORE_INFO("Shader parsed layout: [{}] {} {} {}", location, modifier, type, name);
				}
			//}
		}
	}

}