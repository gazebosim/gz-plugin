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

#include <ignition/plugin/Loader.hh>
#include <ignition/plugin/config.hh>

#include <ignition/plugin/SpecializedPluginPtr.hh>

/////////////////////////////////////////////////
TEST(Loader, InitialNoInterfacesImplemented)
{
  ignition::plugin::Loader loader;
  EXPECT_EQ(0u, loader.InterfacesImplemented().size());
}

/////////////////////////////////////////////////
TEST(Loader, LoadNonexistantLibrary)
{
  ignition::plugin::Loader loader;
  EXPECT_TRUE(loader.LoadLibrary("/path/to/libDoesNotExist.so").empty());
}

/////////////////////////////////////////////////
TEST(Loader, LoadNonLibrary)
{
  ignition::plugin::Loader loader;
  EXPECT_TRUE(loader.LoadLibrary(std::string(IGN_PLUGIN_SOURCE_DIR)
                             + "/core/src/Plugin.cc").empty());
}

/////////////////////////////////////////////////
TEST(Loader, LoadNonPluginLibrary)
{
  ignition::plugin::Loader loader;
  EXPECT_TRUE(loader.LoadLibrary(IGN_PLUGIN_LIB).empty());
}

/////////////////////////////////////////////////
TEST(Loader, InstantiateUnloadedPlugin)
{
  ignition::plugin::Loader loader;
  ignition::plugin::PluginPtr plugin =
      loader.Instantiate("plugin::that::is::not::loaded");
  EXPECT_FALSE(plugin);
}

class SomeInterface { };

/////////////////////////////////////////////////
TEST(Loader, InstantiateSpecializedPlugin)
{
  ignition::plugin::Loader loader;

  // This makes sure that Loader::Instantiate can compile
  auto plugin = loader.Instantiate<
      ignition::plugin::SpecializedPluginPtr<SomeInterface>>("fake::plugin");
  EXPECT_FALSE(plugin);
}

/////////////////////////////////////////////////
int main(int argc, char **argv)
{
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}
