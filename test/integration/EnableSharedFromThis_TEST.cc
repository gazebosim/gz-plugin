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

#include <ignition/plugin/EnablePluginFromThis.hh>
#include <ignition/plugin/Loader.hh>
#include <ignition/plugin/SpecializedPluginPtr.hh>

#include "../plugins/DummyPlugins.hh"

/////////////////////////////////////////////////
TEST(EnableSharedFromThis, BasicInstantiate)
{
  ignition::plugin::Loader pl;
  pl.LoadLibrary(IGNDummyPlugins_LIB);

  ignition::plugin::PluginPtr plugin =
      pl.Instantiate("test::util::DummyMultiPlugin");
  ASSERT_TRUE(plugin);

  auto *fromThisInterface =
      plugin->QueryInterface<ignition::plugin::EnablePluginFromThis>();
  EXPECT_TRUE(fromThisInterface);

  ignition::plugin::PluginPtr fromThis = fromThisInterface->PluginFromThis();
  EXPECT_EQ(plugin, fromThis);


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
TEST(EnableSharedFromThis, TemplatedInstantiate)
{
  ignition::plugin::Loader pl;
  pl.LoadLibrary(IGNDummyPlugins_LIB);

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
int main(int argc, char **argv)
{
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}
