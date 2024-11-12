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

		for (int i = 0; i < 50; i++)
		{
			Command* cmd = EntityCommand::Create(entity, glm::vec3(i, 0.0f, 0.0f));
			CommandStack::Push(cmd);
		}

		EXPECT_EQ(CommandStack::Count(), 50);
		EXPECT_EQ(CommandStack::Index(), 0);

		for (int i = 0; i < 50; i++)
		{
			EntityCommand* cmd = reinterpret_cast<EntityCommand*>(CommandStack::Pop());
			EXPECT_EQ(cmd->GetValue().x, 49 - i);
		}

		EXPECT_EQ(CommandStack::Count(), 0);
		EXPECT_EQ(CommandStack::Index(), 0);
	}
}