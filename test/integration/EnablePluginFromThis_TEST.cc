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

#include <ignition/plugin/EnablePluginFromThis.hh>
#include <ignition/plugin/Loader.hh>
#include <ignition/plugin/SpecializedPluginPtr.hh>
#include <ignition/plugin/WeakPluginPtr.hh>

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
TEST(EnablePluginFromThis, BasicInstantiate)
{
  ignition::plugin::Loader pl;
  std::string libPath;
  ASSERT_TRUE(IgnDummyPluginLib(libPath));
  pl.LoadLib(libPath);

  ignition::plugin::PluginPtr plugin =
      pl.Instantiate("test::util::DummyMultiPlugin");
  ASSERT_TRUE(plugin);

  auto *fromThisInterface =
      plugin->QueryInterface<ignition::plugin::EnablePluginFromThis>();
  EXPECT_TRUE(fromThisInterface);

  ignition::plugin::PluginPtr fromThis = fromThisInterface->PluginFromThis();
  EXPECT_EQ(plugin, fromThis);

  ignition::plugin::ConstPluginPtr constFromThis =
      static_cast<const ignition::plugin::EnablePluginFromThis*>(
        fromThisInterface)->PluginFromThis();
  EXPECT_EQ(constFromThis, fromThis);

  test::util::DummyGetPluginInstancePtr *getInstance =
      plugin->QueryInterface<test::util::DummyGetPluginInstancePtr>();
  ASSERT_TRUE(getInstance);
  std::shared_ptr<void> ptr = getInstance->PluginInstancePtr();
  EXPECT_NE(nullptr, ptr);


  // Note: the DummySinglePlugin class does not inherit EnablePluginFromThis
  plugin = pl.Instantiate("test::util::DummySinglePlugin");
  ASSERT_TRUE(plugin);

  fromThisInterface =
      plugin->QueryInterface<ignition::plugin::EnablePluginFromThis>();
  EXPECT_EQ(nullptr, fromThisInterface);
}

/////////////////////////////////////////////////
using MySpecializedPluginPtr = ignition::plugin::SpecializedPluginPtr<
  test::util::DummyNameBase,
  test::util::DummyDoubleBase,
  test::util::DummyIntBase
>;

/////////////////////////////////////////////////
TEST(EnablePluginFromThis, TemplatedInstantiate)
{
  ignition::plugin::Loader pl;
  std::string libPath;
  ASSERT_TRUE(IgnDummyPluginLib(libPath));
  pl.LoadLib(libPath);

  MySpecializedPluginPtr plugin =
      pl.Instantiate<MySpecializedPluginPtr>("test::util::DummyMultiPlugin");
  ASSERT_TRUE(plugin);

  auto *fromThisInterface =
      plugin->QueryInterface<ignition::plugin::EnablePluginFromThis>();
  EXPECT_TRUE(fromThisInterface);

  ignition::plugin::PluginPtr fromThis = fromThisInterface->PluginFromThis();
  EXPECT_EQ(plugin, fromThis);


  // Note: the DummySinglePlugin class does not inherit EnablePluginFromThis
  plugin = pl.Instantiate<MySpecializedPluginPtr>(
        "test::util::DummySinglePlugin");
  ASSERT_TRUE(plugin);

  fromThisInterface =
      plugin->QueryInterface<ignition::plugin::EnablePluginFromThis>();
  EXPECT_EQ(nullptr, fromThisInterface);
}

/////////////////////////////////////////////////
TEST(EnablePluginFromThis, LibraryManagement)
{
  std::string libPath;
  ASSERT_TRUE(IgnDummyPluginLib(libPath));
  const std::string &libraryPath = libPath;

  ignition::plugin::WeakPluginPtr weak;

  {
    ignition::plugin::PluginPtr longterm;

    CHECK_FOR_LIBRARY(libraryPath, false);

    {
      ignition::plugin::Loader pl;
      pl.LoadLib(libraryPath);

      CHECK_FOR_LIBRARY(libraryPath, true);

      ignition::plugin::PluginPtr temporary =
          pl.Instantiate("test::util::DummyMultiPlugin");

      auto fromThis =
          temporary->QueryInterface<ignition::plugin::EnablePluginFromThis>();

      longterm = fromThis->PluginFromThis();
      weak = fromThis->PluginFromThis();

      EXPECT_FALSE(weak.IsExpired());
      EXPECT_FALSE(longterm.IsEmpty());
    }

    EXPECT_FALSE(weak.IsExpired());
    EXPECT_FALSE(longterm.IsEmpty());

    CHECK_FOR_LIBRARY(libraryPath, true);
  }

  EXPECT_TRUE(weak.IsExpired());

  CHECK_FOR_LIBRARY(libraryPath, false);
}

/////////////////////////////////////////////////
int main(int argc, char **argv)
{
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}
