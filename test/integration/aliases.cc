/*
 * Copyright (C) 2018 Open Source Robotics Foundation
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

#include <ignition/common/Filesystem.hh>
#include <ignition/common/Util.hh>

#include <ignition/plugin/Loader.hh>

#include "../plugins/DummyPlugins.hh"

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
TEST(Alias, InspectAliases)
{
  ignition::plugin::Loader pl;

  std::string libPath;
  ASSERT_TRUE(IgnDummyPluginLib(libPath));

  // Make sure the expected plugins were loaded.
  std::unordered_set<std::string> pluginNames =
      pl.LoadLib(libPath);
  ASSERT_EQ(1u, pluginNames.count("test::util::DummySinglePlugin"));
  ASSERT_EQ(1u, pluginNames.count("test::util::DummyMultiPlugin"));
  ASSERT_EQ(1u, pluginNames.count("test::util::DummyNoAliasPlugin"));

  EXPECT_EQ(0u, pl.PluginsWithAlias("fake alias").size());
  EXPECT_EQ(1u, pl.PluginsWithAlias("Alternative name").size());
  EXPECT_EQ(2u, pl.PluginsWithAlias("Bar").size());
  EXPECT_EQ(2u, pl.PluginsWithAlias("Baz").size());
  EXPECT_EQ(1u, pl.PluginsWithAlias("Foo").size());
  EXPECT_EQ(1u, pl.PluginsWithAlias("test::util::DummySinglePlugin").size());

  EXPECT_EQ(3u, pl.AliasesOfPlugin("test::util::DummySinglePlugin").size());
  EXPECT_EQ(3u, pl.AliasesOfPlugin("test::util::DummyMultiPlugin").size());
  EXPECT_EQ(0u, pl.AliasesOfPlugin("test::util::DummyNoAliasPlugin").size());
  EXPECT_EQ(0u, pl.AliasesOfPlugin("fake::plugin").size());
}

/////////////////////////////////////////////////
TEST(Alias, ConflictingAlias)
{
  ignition::plugin::Loader pl;

  std::string libPath;
  ASSERT_TRUE(IgnDummyPluginLib(libPath));

  // Make sure the expected plugins were loaded.
  std::unordered_set<std::string> pluginNames =
      pl.LoadLib(libPath);
  ASSERT_EQ(1u, pluginNames.count("test::util::DummySinglePlugin"));
  ASSERT_EQ(1u, pluginNames.count("test::util::DummyMultiPlugin"));
  ASSERT_EQ(1u, pluginNames.count("test::util::DummyNoAliasPlugin"));

  // This alias conflicts between DummySinglePlugin and DummyMultiPlugin
  ignition::plugin::PluginPtr attempt = pl.Instantiate("Bar");
  EXPECT_TRUE(attempt.IsEmpty());

  // This alias conflicts between DummySinglePlugin and DummyMultiPlugin
  attempt = pl.Instantiate("Baz");
  EXPECT_TRUE(attempt.IsEmpty());

  // This alias refers only to DummySinglePlugin
  attempt = pl.Instantiate("Alternative name");
  EXPECT_FALSE(attempt.IsEmpty());

  auto * interface = attempt->QueryInterface<test::util::DummyNameBase>();
  ASSERT_NE(nullptr, interface);
  EXPECT_EQ("DummySinglePlugin", interface->MyNameIs());

  // This alias refers only to DummyMultiPlugin
  attempt = pl.Instantiate("Foo");
  EXPECT_FALSE(attempt.IsEmpty());

  interface = attempt->QueryInterface<test::util::DummyNameBase>();
  ASSERT_NE(nullptr, interface);
  EXPECT_EQ("DummyMultiPlugin", interface->MyNameIs());

  attempt = pl.Instantiate("not a plugin");
  EXPECT_TRUE(attempt.IsEmpty());
}


/////////////////////////////////////////////////
int main(int argc, char **argv)
{
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}
