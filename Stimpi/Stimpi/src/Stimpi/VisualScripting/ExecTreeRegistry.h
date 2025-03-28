#pragma once

#include "Stimpi/Core/Core.h"
#include "Stimpi/Core/UUID.h"
#include "Stimpi/Utils/FilePath.h"
#include "Stimpi/VisualScripting/ExecTree.h"

/**
 * Look for .egh files and register found "compiled" node graphs - ExecTrees, on Project load.
 * When node graph is compiled successfully, register the resulting ExecTree.
 * Updated registered ExecTrees when re-compiling the same node graph.
 * Assets or "users" of the ExecTree can lookup for the ExecTree based on its UUID
 * 
 * Name of the .egh file is the UUID that the ExecTree is registered for
 */

namespace Stimpi
{
	using ExecTreeRegistryType = std::unordered_map<UUID, std::shared_ptr<ExecTree>>;

	class ST_API ExecTreeRegistry
	{
		public:
			static void RegisterExecTree(const UUID& uuid, std::shared_ptr<ExecTree> execTree);
			static std::shared_ptr<ExecTree> GetExecTree(const UUID& uuid);

			static void LoadProjectExecTrees(const FilePath& dirPath);

			static void SaveExecTree(const UUID& uuid, ExecTree* execTree);
	};
}