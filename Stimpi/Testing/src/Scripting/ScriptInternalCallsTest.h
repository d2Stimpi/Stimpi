#pragma once

#include "Stimpi/Scripting/ScriptEngine.h"

#include <gtest/gtest.h>

using namespace Stimpi;

namespace StimpiTest
{
	class ScriptInternalCallsTest : public testing::Test
	{
	public:
		void SetUp() override;
		void TearDown() override;
	};
}