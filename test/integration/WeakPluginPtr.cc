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

#include <ignition/plugin/WeakPluginPtr.hh>
#include <ignition/plugin/Loader.hh>

#include "../plugins/DummyPlugins.hh"
#include "utils.hh"

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
TEST(WeakPluginPtr, Lifecycle)
{
  std::string libDir;
  ASSERT_TRUE(IgnDummyPluginLib(libDir));
  const std::string &libraryPath = libDir;

  ignition::plugin::WeakPluginPtr weak;

  CHECK_FOR_LIBRARY(libraryPath, false);

  {
    ignition::plugin::Loader pl;
    pl.LoadLib(libraryPath);
    CHECK_FOR_LIBRARY(libraryPath, true);

    ignition::plugin::PluginPtr plugin =
        pl.Instantiate("test::util::DummyMultiPlugin");

    weak = plugin;

    EXPECT_FALSE(weak.IsExpired());

    ignition::plugin::PluginPtr copy = weak.Lock();

    test::util::DummyIntBase* base =
        copy->QueryInterface<test::util::DummyIntBase>();

    EXPECT_TRUE(base);
    EXPECT_EQ(5, base->MyIntegerValueIs());
    EXPECT_EQ(plugin->QueryInterface<test::util::DummyIntBase>(), base);

    CHECK_FOR_LIBRARY(libraryPath, true);
  }

  CHECK_FOR_LIBRARY(libraryPath, false);

  EXPECT_TRUE(weak.IsExpired());

  ignition::plugin::PluginPtr empty = weak.Lock();
  EXPECT_TRUE(empty.IsEmpty());
  EXPECT_FALSE(empty->QueryInterface<test::util::DummyIntBase>());
}

/////////////////////////////////////////////////
TEST(WeakPluginPtr, CopyMove)
{
  ignition::plugin::Loader pl;
  std::string libPath;
  ASSERT_TRUE(IgnDummyPluginLib(libPath));
  pl.LoadLib(libPath);

  ignition::plugin::PluginPtr plugin =
      pl.Instantiate("test::util::DummyMultiPlugin");

  ignition::plugin::WeakPluginPtr weakConstructFromPlugin(plugin);
  EXPECT_EQ(plugin, weakConstructFromPlugin.Lock());

  ignition::plugin::WeakPluginPtr weakCopyFromOther(
        weakConstructFromPlugin);
  EXPECT_EQ(plugin, weakConstructFromPlugin.Lock());

  ignition::plugin::WeakPluginPtr weakCopyAssignFromOther;
  weakCopyAssignFromOther = weakConstructFromPlugin;
  EXPECT_EQ(plugin, weakCopyAssignFromOther.Lock());

  ignition::plugin::WeakPluginPtr weakMoveFromOther(
        std::move(weakCopyFromOther));
  EXPECT_EQ(plugin, weakMoveFromOther.Lock());

  ignition::plugin::WeakPluginPtr weakMoveAssignFromOther;
  weakMoveAssignFromOther = std::move(weakCopyAssignFromOther);
  EXPECT_EQ(plugin, weakMoveAssignFromOther.Lock());

  ignition::plugin::WeakPluginPtr weakAssignFromPlugin;
  weakAssignFromPlugin = plugin;
  EXPECT_EQ(plugin, weakAssignFromPlugin.Lock());
}

/////////////////////////////////////////////////
int main(int argc, char **argv)
{
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}
