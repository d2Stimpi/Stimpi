#include "stpch.h"
#include "Stimpi/Asset/ShaderImporter.h"

#include "Stimpi/Log.h"
#include "Stimpi/Core/Project.h"
#include "Stimpi/VisualScripting/ExecTreeSerializer.h"


namespace Stimpi
{
	// internal only uniforms that are skipped
	std::vector<std::string> s_FilterUnifromNames = { "u_ViewProjection", "u_texture" };
	// skip standard layout data
	std::vector<std::string> s_FilterLayoutNames = { "aPos", "aColor", "aTexCoord" };


	std::shared_ptr<Stimpi::Asset> ShaderImporter::ImportShader(AssetHandle handle, const AssetMetadata& metadata)
	{
		FilePath assetPath = Project::GetAssestsDir() / metadata.m_FilePath.string();
		return std::static_pointer_cast<Asset>(LoadShader(assetPath, handle));
	}

	std::shared_ptr<Stimpi::Shader> ShaderImporter::LoadShader(const FilePath& filePath, AssetHandle handle)
	{
		// 1. Read shader file
		std::string vertexShaderCode;
		std::string fragmentShaderCode;
		std::ifstream shaderFile;
		ShaderInfo shaderInfo;

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
					// 2. Parse shader layout information
					ParseShaderByLine(line, shaderInfo);
					vertexStream << line << std::endl;
				}
				else
				{
					ParseShaderByLine(line, shaderInfo);
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
		
		shaderInfo.m_Name = filePath.GetPath().stem().string();

		// 3. Create shader asset
		auto shader = Shader::Create(shaderInfo, vertexShaderCode, fragmentShaderCode);

		// 4. Try to parse the shader Graph if available
		LoadShaderGraph(shader.get(), filePath, handle);

		return shader;
	}

	Stimpi::ShaderInfo ShaderImporter::ParseShaderByLine(const std::string& line, ShaderInfo& shaderInfo)
	{
		size_t pos = line.find("layout(location =");
		if (pos != std::string::npos)
		{
			int location = std::stoi(line.substr(17, line.length() - 17));
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
			}
		}

		// Check for uniform
		pos = line.find("uniform");
		if (pos != std::string::npos)
		{
			pos += strlen("uniform");
			std::string substr = line.substr(pos + 1, line.length() - pos - 1);
			const char* delimiter = " ";
			char* token = std::strtok(substr.data(), delimiter);

			std::string type = "";
			std::string name = "";

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

			// Filter out "internal" uniforms
			if (std::find(s_FilterUnifromNames.begin(), s_FilterUnifromNames.end(), name) == s_FilterUnifromNames.end())
				shaderInfo.m_Uniforms.emplace_back(StringToShaderType(type), name);
		}

		return shaderInfo;
	}

	bool ShaderImporter::LoadShaderGraph(Shader* shader, const FilePath& filePath, AssetHandle handle)
	{
		std::string fileName = fmt::format("{}.egh", handle);
		FilePath graphPath = FilePath(Project::GetResourcesSubdir(Project::Subdir::VisualScripting) / fileName);
		if (graphPath.Exists())
		{
			// Deserialize the ExecTree
			std::shared_ptr<ExecTree> execTree = std::make_shared<ExecTree>(fileName);
			ExecTreeSerializer serializer(execTree.get());
			if (serializer.Deseriealize(graphPath.string()))
			{
				shader->SetCustomExecTree(execTree);
				return true;
			}
		}

		return false;
	}

}