/*
 * Copyright (C) 2017 Open Source Robotics Foundation
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
*/

#include <algorithm>
#include <gtest/gtest.h>
#include "ignition/common/PluginLoader.hh"


/////////////////////////////////////////////////
TEST(PluginLoader, InitialNoSearchPaths)
{
  ignition::common::PluginLoader pm;
  EXPECT_EQ(0, pm.SearchPaths().size());
}

/////////////////////////////////////////////////
TEST(PluginLoader, InitialNoInterfacesImplemented)
{
  ignition::common::PluginLoader pm;
  EXPECT_EQ(0, pm.InterfacesImplemented().size());
}

/////////////////////////////////////////////////
TEST(PluginLoader, AddOneSearchPath)
{
  ignition::common::PluginLoader pm;
  pm.AddSearchPath("./");
  ASSERT_EQ(1, pm.SearchPaths().size());
  EXPECT_EQ("./", pm.SearchPaths()[0]);
}

/////////////////////////////////////////////////
TEST(PluginLoader, SearchPathGetsTrailingSlash)
{
  ignition::common::PluginLoader pm;
  pm.AddSearchPath("/usr/local/lib");
  ASSERT_EQ(1, pm.SearchPaths().size());
  EXPECT_EQ("/usr/local/lib/", pm.SearchPaths()[0]);
}

/////////////////////////////////////////////////
TEST(PluginLoader, SearchPathUsesForwardSlashes)
{
  ignition::common::PluginLoader pm;
  pm.AddSearchPath("C:\\user\\alice\\gazebolibs\\");
  ASSERT_EQ(1, pm.SearchPaths().size());
  EXPECT_EQ("C:/user/alice/gazebolibs/", pm.SearchPaths()[0]);
}

/////////////////////////////////////////////////
TEST(PluginLoader, LoadNonexistantLibrary)
{
  ignition::common::PluginLoader pm;
  pm.AddSearchPath("../util");
  EXPECT_FALSE(pm.LoadLibrary("DoesNotExist"));
}


/////////////////////////////////////////////////
int main(int argc, char **argv)
{
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}
