#include "SceneTest/EntitySortingTest.h"

#include "Stimpi/Scripting/ScriptEngine.h"
#include "Stimpi/Scene/ResourceManager.h"

#include "SceneTest/SceneTestUtils.h"

namespace StimpiTest
{

	static void VerifyEntityOrder(std::vector<uint32_t>& data, std::vector<Entity>& cmp)
	{
		if (data.size() != cmp.size())
			return;

		for (int i = 0; i < data.size(); i++)
		{
			EXPECT_EQ(data[i], (uint32_t)cmp[i]);
		}
	}



	void EntitySortingTest::SetUp()
	{
		ResourceManager::LoadDefaultProject();
		ScriptEngine::Init();

		m_Scene = new Scene();
		EntityFactory::SetContext(m_Scene);
	}

	void EntitySortingTest::TearDown()
	{
		ScriptEngine::Shutdown();

		delete m_Scene;
		m_Scene = nullptr;
	}

	TEST_F(EntitySortingTest, SimpleSortTest)
	{
		std::vector<Entity> entities;

		auto& sorter = m_Scene->GetEntitySorter();

		auto& entityGroups = sorter.GetEntityGroups();
		auto& axisSortedEntites = sorter.GetAxisSortedEntites();

		EXPECT_EQ(0, entityGroups.size());
		EXPECT_EQ(0, axisSortedEntites.size());

		entities.push_back(EntityFactory::CreateEntity({ ComponentType::QUAD, ComponentType::SPRITE }));
		entities.push_back(EntityFactory::CreateEntity({ ComponentType::QUAD, ComponentType::SPRITE }));
		entities.push_back(EntityFactory::CreateEntity({ ComponentType::QUAD, ComponentType::SPRITE }));
		entities.push_back(EntityFactory::CreateEntity({ ComponentType::QUAD, ComponentType::SPRITE }));
		entities.push_back(EntityFactory::CreateEntity({ ComponentType::QUAD, ComponentType::SPRITE }));

		EXPECT_EQ(0, entityGroups.size());
		EXPECT_EQ(5, axisSortedEntites.size());
		VerifyEntityOrder(axisSortedEntites, entities);

		for (auto entity : entities)
		{
			m_Scene->RemoveEntity(entity);
		}

		EXPECT_EQ(0, entityGroups.size());
		EXPECT_EQ(0, axisSortedEntites.size());
	}

}