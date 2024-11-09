#include "CmdStackTest.h"

#include "Stimpi/Cmd/CommandStack.h"
#include "SceneTest/SceneTestUtils.h"

using namespace Stimpi;

namespace StimpiTest
{

	void CmdStackTest::SetUp()
	{

	}

	void CmdStackTest::TearDown()
	{

	}

	TEST_F(CmdStackTest, PopulateCmdStack)
	{
		Entity entity = EntityFactory::CreateEntity({ ComponentType::QUAD });
		Command* cmd = TranslateCommand::Create(entity, glm::vec3(0.0f, 0.0f, 0.0f));

		CommandStack::Push(cmd);

		EXPECT_EQ(CommandStack::Count(), 1);
		EXPECT_EQ(CommandStack::Index(), 1);
	}
}