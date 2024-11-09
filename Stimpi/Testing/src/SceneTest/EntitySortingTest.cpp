#include "SceneTest/EntitySortingTest.h"

#include "SceneTest/SceneTestUtils.h"

#include "Stimpi/Scene/Component.h"
#include "Stimpi/Scene/SceneManager.h"

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

	static void VerifyEntityOrder(std::vector<EntitySortData>& data, std::vector<uint32_t> cmp)
	{
		if (data.size() != cmp.size())
			return;

		for (int i = 0; i < data.size(); i++)
		{
			EXPECT_EQ(data[i].m_EntityID, cmp[i]);
		}
	}

	void EntitySortingTest::SetUp()
	{
		SceneManager::Instance()->NewScene();
		m_Scene = SceneManager::Instance()->GetActiveScene();
		EntityFactory::SetContext(m_Scene);

		// Clear all sorting data before running a Test fixture
		auto& sorter = m_Scene->GetEntitySorter();
		sorter.ClearAllSortingData();
	}

	void EntitySortingTest::TearDown()
	{
		/*delete m_Scene;
		m_Scene = nullptr;*/
	}

	Entity EntitySortingTest::AddSortingGroupEntity(const std::string& layerName, uint32_t order)
	{
		Entity entity = EntityFactory::CreateEntity({
			ComponentType::QUAD,
			ComponentType::SPRITE,
			ComponentType::SORTING_GROUP });

		SortingGroupComponent& group = entity.GetComponent<SortingGroupComponent>();
		group.m_SortingLayerName = layerName;
		group.m_OrderInLayer = order;

		m_Scene->UpdateLayerSorting(entity);

		return entity;
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

		entities.push_back(EntityFactory::CreateEntity({ 
			ComponentType::QUAD,
			ComponentType::SPRITE,
			ComponentType::SORTING_GROUP }));

		EXPECT_EQ(1, entityGroups.size());
		EXPECT_EQ(1, entityGroups["Default"].m_Entities.size());
		EXPECT_EQ(5, axisSortedEntites.size());
		VerifyEntityOrder(axisSortedEntites, entities);

		for (auto entity : entities)
		{
			m_Scene->RemoveEntity(entity);
		}

		EXPECT_EQ(1, entityGroups.size());
		EXPECT_EQ(0, entityGroups["Default"].m_Entities.size());
		EXPECT_EQ(0, axisSortedEntites.size());
	}

	TEST_F(EntitySortingTest, SortingGroup_CreateComponents)
	{
		std::vector<Entity> entities;

		auto& sorter = m_Scene->GetEntitySorter();

		auto& entityGroups = sorter.GetEntityGroups();
		auto& axisSortedEntites = sorter.GetAxisSortedEntites();

		entities.push_back(AddSortingGroupEntity("Background", 5));
		entities.push_back(AddSortingGroupEntity("Background", 2));
		entities.push_back(AddSortingGroupEntity("Background", 1));

		// "Default" sorting group will always exist as we create SortingGroupComponent
		// with default values and change it afterwards
		EXPECT_EQ(2, entityGroups.size());
		EXPECT_EQ(3, entityGroups["Background"].m_Entities.size());
		EXPECT_EQ(0, axisSortedEntites.size());
		VerifyEntityOrder(entityGroups["Background"].m_Entities, {3, 2, 1});

		m_Scene->RemoveEntity(entities[entities.size() - 1]);
		entities.pop_back();

		EXPECT_EQ(2, entityGroups.size());
		EXPECT_EQ(2, entityGroups["Background"].m_Entities.size());
		EXPECT_EQ(0, axisSortedEntites.size());
		VerifyEntityOrder(entityGroups["Background"].m_Entities, { 2, 1 });
	}

	// TODO: test when entity adds/removes components

}