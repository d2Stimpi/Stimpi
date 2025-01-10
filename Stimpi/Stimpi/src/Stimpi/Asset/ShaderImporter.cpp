#include "stpch.h"
#include "Stimpi/Asset/ShaderImporter.h"

#include "Stimpi/Log.h"

namespace Stimpi
{

	std::shared_ptr<Stimpi::Asset> ShaderImporter::ImportShader(AssetHandle handle, const AssetMetadata& metadata)
	{
		return nullptr;
	}

	std::shared_ptr<Stimpi::Shader> ShaderImporter::LoadShader(const FilePath& filePath)
	{
		// 1. Read shader file
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

			shaderFile.open(filePath.string());

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
					ParseVertexShaderByLine(line);
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
			ST_CORE_ERROR("ShaderImporter: failed to read shader file {0} - error: {1}\n", filePath.string(), e.what());
		}

		// 2. Parse shader layout information
		
		// 3. Create shader asset
		auto shader = Shader::Create({}, vertexShaderCode, fragmentShaderCode);

		return nullptr;
	}

	Stimpi::ShaderInfo ShaderImporter::ParseVertexShaderByLine(const std::string& line)
	{
		ShaderInfo shaderInfo;
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

				shaderInfo.m_ShaderLayout.m_Data.emplace_back(StringToShaderType(type), name);
				//ST_CORE_INFO("Shader parsed layout: [{}] {} {} {}", location, modifier, type, name);
			}
			//}
		}

		return shaderInfo;
	}

}