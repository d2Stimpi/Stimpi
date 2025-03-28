#include "stpch.h"
#include "Stimpi/VisualScripting/ExecTreeRegistry.h"

#include "Stimpi/Log.h"
#include "Stimpi/VisualScripting/ExecTreeSerializer.h"
#include "Stimpi/Core/Project.h"

namespace Stimpi
{
	ExecTreeRegistryType s_ExecTreeRegistry;


	void ExecTreeRegistry::RegisterExecTree(const UUID& uuid, std::shared_ptr<ExecTree> execTree)
	{
		if (execTree == nullptr)
			return;

		s_ExecTreeRegistry[uuid] = execTree;
		ST_CORE_INFO("[ExecTreeRegistry] Registered ExecTree: UUID = {}", uuid);

		SaveExecTree(uuid, execTree.get());
	}

	std::shared_ptr<ExecTree> ExecTreeRegistry::GetExecTree(const UUID& uuid)
	{
		auto found = s_ExecTreeRegistry.find(uuid);
		if (found != s_ExecTreeRegistry.end())
			return found->second;
		
		return nullptr;
	}

	void ExecTreeRegistry::LoadProjectExecTrees(const FilePath& dirPath)
	{
		if (!dirPath.Exists())
		{
			ST_CORE_WARN("[ExecTreeRegistry] Load ExecTrees failed - VisualScripting subfolder not found!");
			return;
		}

		// Make sure that NodeMethods are loaded
		NNodeMethodRegistry::PopulateMethods();

		for (auto& directoryEntry : std::filesystem::directory_iterator(dirPath))
		{
			UUID uuid = 0;
			const auto& filePath = directoryEntry.path();
			std::string filenameStr = filePath.filename().string();

			// Filter out non .egh extension files
			if (filePath.extension() != ".egh")
				continue;

			// Try if name is valid
			try
			{
				uuid = std::stoull(filePath.filename().stem().string());
			}
			catch (std::invalid_argument const& ex)
			{
				ST_CORE_WARN("[ExecTreeRegistry] LoadProjectExecTrees: wrong file name format - {}", filePath.filename());
				continue;
			}

			// Deserialize the ExecTree
			std::shared_ptr<ExecTree> execTree = std::make_shared<ExecTree>(filenameStr);
			ExecTreeSerializer serializer(execTree.get());
			if (serializer.Deseriealize(filePath.string()))
			{
				
				RegisterExecTree(uuid, execTree);
			}
		}
	}

	void ExecTreeRegistry::SaveExecTree(const UUID& uuid, ExecTree* execTree)
	{
		if (execTree == nullptr)
			return;

		ExecTreeSerializer execTreeSerializer(execTree);
		std::string fileName = fmt::format("{}.egh", uuid);
		FilePath path = Project::GetResourcesSubdir(Project::Subdir::VisualScripting) / fileName;
		execTreeSerializer.Serialize(path);
	}

}