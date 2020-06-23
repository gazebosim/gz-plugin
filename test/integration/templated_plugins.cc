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

// Defining this macro before including ignition/plugin/SpecializedPluginPtr.hh
// allows us to test that the high-speed routines are being used to access the
// specialized plugin interfaces.
#define IGNITION_UNITTEST_SPECIALIZED_PLUGIN_ACCESS

#include <gtest/gtest.h>

#include <ignition/common/Filesystem.hh>
#include <ignition/common/Util.hh>

#include <ignition/plugin/Loader.hh>
#include <ignition/plugin/SpecializedPluginPtr.hh>

#include "../plugins/TemplatedPlugins.hh"

using namespace test::plugins;

/////////////////////////////////////////////////
bool IgnTemplatedPluginLib(std::string &_path)
{
#ifdef IGNTemplatedPlugins_LIB
  _path = IGNTemplatedPlugins_LIB;
  return true;
#else
  std::string dataDir;
  if (ignition::common::env("TEST_SRCDIR", dataDir))
  {
    _path = ignition::common::joinPaths(dataDir,
        "__main__/ign_plugin/test/IGNTemplatedPlugins.so");
    return true;
  }
#endif
  return false;
}

/////////////////////////////////////////////////
TEST(TemplatedPlugins, InterfaceCount)
{
  ignition::plugin::Loader pl;
  std::string libPath;
  ASSERT_TRUE(IgnTemplatedPluginLib(libPath));
  pl.LoadLib(libPath);

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
using SetAndGetPluginPtr = ignition::plugin::SpecializedPluginPtr<
    TemplatedGetInterface<T>,
    TemplatedSetInterface<T> >;

/////////////////////////////////////////////////
template <typename T>
void TestSetAndGet(const ignition::plugin::Loader &_pl,
                   const T &_valueToUse)
{
  using GetInterface = TemplatedGetInterface<T>;
  using SetInterface = TemplatedSetInterface<T>;

  for (const std::string &pluginName : _pl.PluginsImplementing<GetInterface>())
  {
    const SetAndGetPluginPtr<T> plugin = _pl.Instantiate(pluginName);
    ASSERT_TRUE(static_cast<bool>(plugin));

    EXPECT_EQ(pluginName, *plugin->Name());

    usedSpecializedInterfaceAccess = false;
    ASSERT_TRUE(plugin->template HasInterface<SetInterface>());
    EXPECT_TRUE(usedSpecializedInterfaceAccess);

    usedSpecializedInterfaceAccess = false;
    ASSERT_TRUE(plugin->template HasInterface<GetInterface>());
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
  ignition::plugin::Loader pl;
  std::string libPath;
  ASSERT_TRUE(IgnTemplatedPluginLib(libPath));
  pl.LoadLib(libPath);

  TestSetAndGet<int>(pl, 120);
  TestSetAndGet<std::string>(pl, "some amazing string");
}


/////////////////////////////////////////////////
int main(int argc, char **argv)
{
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}
