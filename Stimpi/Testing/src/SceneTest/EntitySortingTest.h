#pragma once

#include "Stimpi/Scene/Scene.h"

#include <gtest/gtest.h>

using namespace Stimpi;

namespace StimpiTest
{
	class Stimpi::Entity;

	class EntitySortingTest : public testing::Test
	{
	public:
		void SetUp() override;
		void TearDown() override;

		Entity AddSortingGroupEntity(const std::string& layerName, uint32_t order);

	protected:
		Scene* m_Scene;
	};
}