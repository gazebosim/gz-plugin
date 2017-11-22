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
#include "ignition/common/SystemPaths.hh"

#include "ignition/common/config.hh"
#include "test_config.h"

/////////////////////////////////////////////////
TEST(PluginLoader, InitialNoInterfacesImplemented)
{
  ignition::common::PluginLoader pm;
  EXPECT_EQ(0u, pm.InterfacesImplemented().size());
}

/////////////////////////////////////////////////
TEST(PluginLoader, LoadNonexistantLibrary)
{
  ignition::common::PluginLoader pm;
  EXPECT_TRUE(pm.LoadLibrary("/path/to/libDoesNotExist.so").empty());
}

/////////////////////////////////////////////////
TEST(PluginLoader, LoadNonLibrary)
{
  std::string projectPath(PROJECT_BINARY_PATH);
  ignition::common::PluginLoader pm;
  EXPECT_TRUE(pm.LoadLibrary(projectPath + "/test_config.h").empty());
}

/////////////////////////////////////////////////
TEST(PluginLoader, LoadNonPluginLibrary)
{
  std::string projectPath(PROJECT_BINARY_PATH);
  std::string libraryName("ignition-common");
  libraryName += std::to_string(IGNITION_COMMON_MAJOR_VERSION);

  ignition::common::SystemPaths sp;
  sp.AddPluginPaths(projectPath + "/src");
  std::string path = sp.FindSharedLibrary(libraryName);
  ASSERT_FALSE(path.empty());

  ignition::common::PluginLoader pm;
  EXPECT_TRUE(pm.LoadLibrary(path).empty());
}

/////////////////////////////////////////////////
int main(int argc, char **argv)
{
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}
