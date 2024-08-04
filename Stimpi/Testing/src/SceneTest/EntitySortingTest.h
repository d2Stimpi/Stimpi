#pragma once

#include "Stimpi/Scene/Scene.h"

#include <gtest/gtest.h>

using namespace Stimpi;

namespace StimpiTest
{
	class EntitySortingTest : public testing::Test
	{
	public:
		void SetUp() override;
		void TearDown() override;

	protected:
		Scene* m_Scene;
	};
}