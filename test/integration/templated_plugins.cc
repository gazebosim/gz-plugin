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

// Defining this macro before including ignition/common/SpecializedPluginPtr.hh
// allows us to test that the high-speed routines are being used to access the
// specialized plugin interfaces.
#define IGNITION_UNITTEST_SPECIALIZED_PLUGIN_ACCESS

#include <gtest/gtest.h>

#include <ignition/common/PluginLoader.hh>
#include <ignition/common/SystemPaths.hh>
#include <ignition/common/SpecializedPluginPtr.hh>

#include "test_config.h"
#include "plugins/TemplatePlugin.hh"

using namespace test::plugins;

// This macro provides the path to the directory containing the test plugins.
// DETAIL_IGN_DUMMY_PLUGIN_PATH is defined in test/integration/CMakeLists.txt
#define IGN_DUMMY_PLUGIN_PATH \
  DETAIL_IGN_DUMMY_PLUGIN_PATH

/////////////////////////////////////////////////
TEST(TemplatedPlugins, InterfaceCount)
{
  ignition::common::SystemPaths sp;
  sp.AddPluginPaths(IGN_DUMMY_PLUGIN_PATH);
  const std::string path = sp.FindSharedLibrary("IGNTemplatedPlugins");

  ignition::common::PluginLoader pl;
  pl.LoadLibrary(path);

  const std::size_t getIntCount =
       pl.PluginsImplementing< TemplatedGetInterface<int> >().size();
  EXPECT_EQ(2u, getIntCount);

  const std::size_t setIntCount =
      pl.PluginsImplementing< TemplatedSetInterface<int> >().size();
  EXPECT_EQ(2u, setIntCount);

  const std::size_t getStringCount =
      pl.PluginsImplementing< TemplatedGetInterface<std::string> >().size();
  EXPECT_EQ(2u, getStringCount);

  const std::size_t setStringCount =
      pl.PluginsImplementing< TemplatedSetInterface<std::string> >().size();
  EXPECT_EQ(2u, setStringCount);

  const std::size_t getDoubleCount =
      pl.PluginsImplementing< TemplatedGetInterface<double> >().size();
  EXPECT_EQ(0u, getDoubleCount);

  const std::size_t setDoubleCount =
      pl.PluginsImplementing< TemplatedSetInterface<double> >().size();
  EXPECT_EQ(0u, setDoubleCount);
}

/////////////////////////////////////////////////
template<typename T>
using SetAndGetPluginPtr = ignition::common::SpecializedPluginPtr<
    TemplatedGetInterface<T>,
    TemplatedSetInterface<T> >;

/////////////////////////////////////////////////
template <typename T>
void TestSetAndGet(const ignition::common::PluginLoader &_pl,
                   const T &_valueToUse)
{
  using GetInterface = TemplatedGetInterface<T>;
  using SetInterface = TemplatedSetInterface<T>;

  for (const std::string &pluginName : _pl.PluginsImplementing<GetInterface>())
  {
    const SetAndGetPluginPtr<T> plugin = _pl.Instantiate(pluginName);

    usedSpecializedInterfaceAccess = false;
    // We assume that plugins which provide the setter also provide the setter.
    // This is just for testing convenience.
    ASSERT_TRUE(plugin->template HasInterface<SetInterface>());
    EXPECT_TRUE(usedSpecializedInterfaceAccess);

    usedSpecializedInterfaceAccess = false;
    SetInterface *setter = plugin->template QueryInterface<SetInterface>();
    EXPECT_TRUE(usedSpecializedInterfaceAccess);
    ASSERT_TRUE(setter);

    setter->Set(_valueToUse);
    usedSpecializedInterfaceAccess = false;
    GetInterface *getter = plugin->template QueryInterface<GetInterface>();
    EXPECT_TRUE(usedSpecializedInterfaceAccess);
    ASSERT_TRUE(getter);
    EXPECT_EQ(_valueToUse, getter->Get());
  }
}

/////////////////////////////////////////////////
TEST(TemplatedPlugins, SetAndGet)
{
  ignition::common::SystemPaths sp;
  sp.AddPluginPaths(IGN_DUMMY_PLUGIN_PATH);
  const std::string path = sp.FindSharedLibrary("IGNTemplatedPlugins");

  ignition::common::PluginLoader pl;
  pl.LoadLibrary(path);

  TestSetAndGet<int>(pl, 120);
  TestSetAndGet<std::string>(pl, "some amazing string");
}


/////////////////////////////////////////////////
int main(int argc, char **argv)
{
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}
