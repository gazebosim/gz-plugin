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

#include <gtest/gtest.h>

#include <algorithm>
#include <fstream>

#include <ignition/common/Filesystem.hh>
#include <ignition/common/Util.hh>

#include <ignition/plugin/Loader.hh>
#include <ignition/plugin/config.hh>

#include <ignition/plugin/SpecializedPluginPtr.hh>


/////////////////////////////////////////////////
bool IgnPluginLib(std::string &_path)
{
#ifdef IGN_PLUGIN_LIB
  _path = IGN_PLUGIN_LIB;
  return true;
#else
  std::string dataDir;
  if (ignition::common::env("TEST_SRCDIR", dataDir))
  {
    _path = ignition::common::joinPaths(dataDir,
        "__main__/ign_plugin/core/libignition-plugin2.so");
    return true;
  }
#endif
  return false;
}

/////////////////////////////////////////////////
bool IgnDummyPluginLib(std::string &_path)
{
#ifdef IGNDummyPlugins_LIB
  _path = IGNDummyPlugins_LIB;
  return true;
#else
  std::string dataDir;
  if (ignition::common::env("TEST_SRCDIR", dataDir))
  {
    _path = ignition::common::joinPaths(dataDir,
        "__main__/ign_plugin/test/IGNDummyPlugins.so");
    return true;
  }
#endif
  return false;
}

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
  EXPECT_TRUE(loader.LoadLib("/path/to/libDoesNotExist.so").empty());
  EXPECT_FALSE(loader.ForgetLibrary("/path/to/libDoesNotExist.so"));
}

/////////////////////////////////////////////////
TEST(Loader, LoadNonLibrary)
{
  ignition::plugin::Loader loader;

  std::ofstream tmpOut("test.tmp");
  tmpOut.close();

  EXPECT_TRUE(loader.LoadLib("test.tmp").empty());
}

/////////////////////////////////////////////////
TEST(Loader, LoadNonPluginLibrary)
{
  ignition::plugin::Loader loader;

  std::string libPath;
  ASSERT_TRUE(IgnPluginLib(libPath));

  EXPECT_TRUE(loader.LoadLib(libPath).empty());
}

/////////////////////////////////////////////////
TEST(Loader, InstantiateUnloadedPlugin)
{
  ignition::plugin::Loader loader;
  ignition::plugin::PluginPtr plugin =
      loader.Instantiate("plugin::that::is::not::loaded");
  EXPECT_FALSE(plugin);
  EXPECT_FALSE(loader.ForgetLibraryOfPlugin("plugin::that::is::not::loaded"));
}

class SomeInterface { };

/////////////////////////////////////////////////
TEST(Loader, InstantiateSpecializedPlugin)
{
  ignition::plugin::Loader loader;

  // This makes sure that Loader::Instantiate can compile
  auto plugin = loader.Instantiate<
      ignition::plugin::SpecializedPluginPtr<SomeInterface> >("fake::plugin");
  EXPECT_FALSE(plugin);
}

/////////////////////////////////////////////////
TEST(Loader, DoubleLoad)
{
  // We do this test for code coverage, to test the lines of code that get run
  // when a user asks for a library to be loaded twice.
  ignition::plugin::Loader loader;

  std::string libPath;
  ASSERT_TRUE(IgnDummyPluginLib(libPath));

  loader.LoadLib(libPath);
  const std::size_t interfaceCount = loader.InterfacesImplemented().size();
  EXPECT_LT(0u, interfaceCount);

  loader.LoadLib(libPath);
  EXPECT_EQ(interfaceCount, loader.InterfacesImplemented().size());
}

/////////////////////////////////////////////////
TEST(Loader, ForgetUnloadedLibrary)
{
  // These tests are for line coverage.
  std::string libPath;
  ASSERT_TRUE(IgnDummyPluginLib(libPath));

  // This first test triggers lines for the case that:
  //   1. A library is not loaded, and
  //   2. We tell a loader to forget the library
  ignition::plugin::Loader loader;
  EXPECT_FALSE(loader.ForgetLibrary(libPath));

  // This next test triggers lines for the case that:
  //   1. A library is loaded by some loader in the application, and
  //   2. We tell a *different* loader to forget the library
  ignition::plugin::Loader hasTheLibrary;
  EXPECT_LT(0u, hasTheLibrary.LoadLib(libPath).size());

  EXPECT_FALSE(loader.ForgetLibrary(libPath));
}

/////////////////////////////////////////////////
int main(int argc, char **argv)
{
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}
