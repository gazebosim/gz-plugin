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

// Defining this macro before including ignition/plugin/SpecializedPluginPtr.hh
// allows us to test that the high-speed routines are being used to access the
// specialized plugin interfaces.
#define IGNITION_UNITTEST_SPECIALIZED_PLUGIN_ACCESS

#include <gtest/gtest.h>
#include <string>
#include <vector>
#include <iostream>

#include <ignition/common/Filesystem.hh>
#include <ignition/common/Util.hh>

#include "ignition/plugin/Loader.hh"
#include "ignition/plugin/PluginPtr.hh"
#include "ignition/plugin/SpecializedPluginPtr.hh"

#include "../plugins/DummyPlugins.hh"
#include "utils.hh"

/////////////////////////////////////////////////
bool BadPlugins(std::vector<std::string> &_paths)
{
#ifdef IGNDummyPlugins_LIB
  _paths = std::vector<std::string>{
    IGNBadPluginAPIVersionOld_LIB,
    IGNBadPluginAPIVersionNew_LIB,
    IGNBadPluginAlign_LIB,
    IGNBadPluginNoInfo_LIB,
    IGNBadPluginSize_LIB};
  return true;
#else
  std::string dataDir;
  if (ignition::common::env("TEST_SRCDIR", dataDir))
  {
    _paths = std::vector<std::string>{
      ignition::common::joinPaths(dataDir,
          "__main__/ign_plugin/test/IGNBadPluginAPIVersionOld.so"),
      ignition::common::joinPaths(dataDir,
          "__main__/ign_plugin/test/IGNBadPluginAPIVersionNew.so"),
      ignition::common::joinPaths(dataDir,
          "__main__/ign_plugin/test/IGNBadPluginAlign.so"),
      ignition::common::joinPaths(dataDir,
          "__main__/ign_plugin/test/IGNBadPluginNoInfo.so"),
      ignition::common::joinPaths(dataDir,
          "__main__/ign_plugin/test/IGNBadPluginSize.so")
    };
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
TEST(Loader, LoadBadPlugins)
{
  std::vector<std::string> libraries;
  ASSERT_TRUE(BadPlugins(libraries));
  for (auto const & library : libraries)
  {
    ignition::plugin::Loader pl;

    // Make sure the expected plugins were loaded.
    std::unordered_set<std::string> pluginNames = pl.LoadLib(library);
    EXPECT_TRUE(pluginNames.empty());
  }
}

/////////////////////////////////////////////////
TEST(Loader, LoadExistingLibrary)
{
  ignition::plugin::Loader pl;
  std::string libPath;
  ASSERT_TRUE(IgnDummyPluginLib(libPath));

  // Make sure the expected plugins were loaded.
  std::unordered_set<std::string> pluginNames = pl.LoadLib(libPath);
  ASSERT_EQ(3u, pluginNames.size());
  ASSERT_EQ(1u, pluginNames.count("test::util::DummySinglePlugin"));
  ASSERT_EQ(1u, pluginNames.count("test::util::DummyMultiPlugin"));
  ASSERT_EQ(1u, pluginNames.count("test::util::DummyNoAliasPlugin"));

  std::cout << pl.PrettyStr();

  // Make sure the expected interfaces were loaded.
  EXPECT_EQ(7u, pl.InterfacesImplemented().size());
  EXPECT_EQ(1u, pl.InterfacesImplemented().count(
                    "test::util::DummyDoubleBase"));
  EXPECT_EQ(1u, pl.InterfacesImplemented().count(
                    "test::util::DummyGetObjectBase"));
  EXPECT_EQ(1u, pl.InterfacesImplemented().count("test::util::DummyIntBase"));
  EXPECT_EQ(1u, pl.InterfacesImplemented().count(
                    "test::util::DummySetterBase"));
  EXPECT_EQ(1u, pl.InterfacesImplemented().count(
                    "ignition::plugin::EnablePluginFromThis"));
  EXPECT_EQ(1u, pl.InterfacesImplemented().count(
                    "test::util::DummyGetPluginInstancePtr"));
  EXPECT_EQ(1u, pl.InterfacesImplemented().count("test::util::DummyNameBase"));

  // Make sure the expected number of plugins implements each interface.

  EXPECT_EQ(3u, pl.PluginsImplementing<::test::util::DummyNameBase>().size());
  EXPECT_EQ(3u, pl.PluginsImplementing("test::util::DummyNameBase").size());
  EXPECT_EQ(3u, pl.PluginsImplementing(
                    typeid(test::util::DummyNameBase).name(), false).size());

  EXPECT_EQ(1u, pl.PluginsImplementing<::test::util::DummyDoubleBase>().size());
  EXPECT_EQ(1u, pl.PluginsImplementing("test::util::DummyDoubleBase").size());
  EXPECT_EQ(1u, pl.PluginsImplementing(
                    typeid(test::util::DummyDoubleBase).name(), false).size());

  EXPECT_EQ(3u, pl.AllPlugins().size());

  // Check DummySinglePlugin.
  ignition::plugin::PluginPtr firstPlugin =
      pl.Instantiate("test::util::DummySinglePlugin");
  EXPECT_FALSE(firstPlugin.IsEmpty());
  EXPECT_TRUE(static_cast<bool>(firstPlugin));

  EXPECT_EQ(std::string("test::util::DummySinglePlugin"), *firstPlugin->Name());

  EXPECT_TRUE(firstPlugin->HasInterface<test::util::DummyNameBase>());
  EXPECT_TRUE(firstPlugin->HasInterface("test::util::DummyNameBase"));

  EXPECT_FALSE(firstPlugin->HasInterface<test::util::DummyDoubleBase>());
  EXPECT_FALSE(firstPlugin->HasInterface("test::util::DummyDoubleBase"));

  EXPECT_FALSE(firstPlugin->HasInterface<test::util::DummyIntBase>());
  EXPECT_FALSE(firstPlugin->HasInterface("test::util::DummyIntBase"));

  EXPECT_FALSE(firstPlugin->HasInterface<test::util::DummySetterBase>());
  EXPECT_FALSE(firstPlugin->HasInterface("test::util::DummySetterBase"));

  // Check DummyMultiPlugin.
  ignition::plugin::PluginPtr secondPlugin =
      pl.Instantiate("test::util::DummyMultiPlugin");
  EXPECT_FALSE(secondPlugin.IsEmpty());

  EXPECT_EQ(std::string("test::util::DummyMultiPlugin"), *secondPlugin->Name());

  EXPECT_TRUE(secondPlugin->HasInterface<test::util::DummyNameBase>());
  EXPECT_TRUE(secondPlugin->HasInterface("test::util::DummyNameBase"));

  EXPECT_TRUE(secondPlugin->HasInterface<test::util::DummyDoubleBase>());
  EXPECT_TRUE(secondPlugin->HasInterface("test::util::DummyDoubleBase"));

  EXPECT_TRUE(secondPlugin->HasInterface<test::util::DummyIntBase>());
  EXPECT_TRUE(secondPlugin->HasInterface("test::util::DummyIntBase"));

  EXPECT_TRUE(secondPlugin->HasInterface<test::util::DummySetterBase>());
  EXPECT_TRUE(secondPlugin->HasInterface("test::util::DummySetterBase"));

  // Check that the right interfaces for each plugin exist (or don't exist) and
  // that they return the correct values.

  // DummyNameBase interface by DummySinglePlugin.
  test::util::DummyNameBase* nameBase =
      firstPlugin->QueryInterface<test::util::DummyNameBase>();
  ASSERT_NE(nullptr, nameBase);
  EXPECT_EQ(std::string("DummySinglePlugin"), nameBase->MyNameIs());

  // No DummyDoubleBase interface by DummySinglePlugin.
  test::util::DummyDoubleBase* doubleBase =
      firstPlugin->QueryInterface<test::util::DummyDoubleBase>();
  EXPECT_EQ(nullptr, doubleBase);

  // DummyNameBase interface by DummyMultiPlugin.
  nameBase = secondPlugin->QueryInterface<test::util::DummyNameBase>();
  ASSERT_NE(nullptr, nameBase);
  EXPECT_EQ(std::string("DummyMultiPlugin"), nameBase->MyNameIs());

  // DummyDoubleBase interface by DummyMultiPlugin.
  doubleBase = secondPlugin->QueryInterface<test::util::DummyDoubleBase>();
  ASSERT_NE(nullptr, doubleBase);
  EXPECT_NEAR(3.14159, doubleBase->MyDoubleValueIs(), 1e-8);

  // DummyIntBase interface by DummyMultiPlugin.
  test::util::DummyIntBase* intBase =
      secondPlugin->QueryInterface<test::util::DummyIntBase>();
  EXPECT_EQ(5, intBase->MyIntegerValueIs());

  // DummyGetObjectBase interface by DummyMultiPlugin.
  test::util::DummyGetObjectBase *objectBase =
      secondPlugin->QueryInterface<test::util::DummyGetObjectBase>();
  ASSERT_NE(nullptr, objectBase);
  test::util::DummyObject object = objectBase->GetDummyObject();
  EXPECT_NEAR(doubleBase->MyDoubleValueIs(), object.dummyDouble, 1e-8);
  EXPECT_EQ(intBase->MyIntegerValueIs(), object.dummyInt);
}


/////////////////////////////////////////////////
class SomeInterface { };

using SomeSpecializedPluginPtr =
    ignition::plugin::SpecializedPluginPtr<
        SomeInterface,
        test::util::DummyIntBase,
        test::util::DummySetterBase>;

/////////////////////////////////////////////////
TEST(SpecializedPluginPtr, Construction)
{
  ignition::plugin::Loader pl;
  std::string libPath;
  ASSERT_TRUE(IgnDummyPluginLib(libPath));
  pl.LoadLib(libPath);

  SomeSpecializedPluginPtr plugin(
      pl.Instantiate("test::util::DummyMultiPlugin"));
  EXPECT_FALSE(plugin.IsEmpty());

  EXPECT_EQ(std::string("test::util::DummyMultiPlugin"), *plugin->Name());

  // Make sure the specialized interface is available, that it is accessed using
  // the specialized access, and that it returns the expected value.
  usedSpecializedInterfaceAccess = false;
  test::util::DummyIntBase *fooBase =
      plugin->QueryInterface<test::util::DummyIntBase>();
  EXPECT_TRUE(usedSpecializedInterfaceAccess);
  ASSERT_NE(nullptr, fooBase);
  EXPECT_EQ(5, fooBase->MyIntegerValueIs());

  // Make sure the specialized interface is available and that it is accessed
  // using the specialized access.
  usedSpecializedInterfaceAccess = false;
  test::util::DummySetterBase *setterBase =
      plugin->QueryInterface<test::util::DummySetterBase>();
  EXPECT_TRUE(usedSpecializedInterfaceAccess);
  EXPECT_NE(nullptr, setterBase);

  // Make sure that the interface makes the expected changes to the plugin.
  const int newIntValue = 54321;
  setterBase->SetIntegerValue(newIntValue);
  EXPECT_EQ(newIntValue, fooBase->MyIntegerValueIs());

  // SomeSpecializedPluginPtr is not specialized for DummyDoubleBase, so here we
  // make sure that it is not being retrieved through specialized acccess.
  usedSpecializedInterfaceAccess = false;
  test::util::DummyDoubleBase *doubleBase =
      plugin->QueryInterface<test::util::DummyDoubleBase>();
  EXPECT_FALSE(usedSpecializedInterfaceAccess);
  EXPECT_NE(nullptr, doubleBase);
  EXPECT_NEAR(3.14159, doubleBase->MyDoubleValueIs(), 1e-8);

  // Make sure that the interfaces work together as expected.
  const double newDubValue = 2.718281828459045;
  setterBase->SetDoubleValue(newDubValue);
  EXPECT_NEAR(newDubValue, doubleBase->MyDoubleValueIs(), 1e-8);

  // Make sure that the unexpected interface is not available, even though the
  // plugin was specialized for it. Also make sure that the specialized access
  // is being used.
  usedSpecializedInterfaceAccess = false;
  SomeInterface *someInterface = plugin->QueryInterface<SomeInterface>();
  EXPECT_TRUE(usedSpecializedInterfaceAccess);
  EXPECT_EQ(nullptr, someInterface);
}

TEST(PluginPtr, Empty)
{
  ignition::plugin::PluginPtr empty;
  EXPECT_TRUE(empty.IsEmpty());
  EXPECT_EQ(nullptr, empty->Name());
  EXPECT_FALSE(empty->HasInterface<SomeInterface>());
  EXPECT_FALSE(static_cast<bool>(empty));
  EXPECT_EQ(nullptr, empty->QueryInterfaceSharedPtr<SomeInterface>());
  ignition::plugin::ConstPluginPtr constEmpty;
  EXPECT_EQ(nullptr, constEmpty->QueryInterfaceSharedPtr<SomeInterface>());
}

/////////////////////////////////////////////////
template <typename PluginPtrType1, typename PluginPtrType2>
void TestSetAndMapUsage(
    const ignition::plugin::Loader &loader,
    const ignition::plugin::PluginPtr &plugin)
{
  PluginPtrType1 plugin1 = plugin;
  PluginPtrType2 plugin2 = plugin1;

  EXPECT_TRUE(plugin1 == plugin);
  EXPECT_TRUE(plugin1 == plugin2);
  EXPECT_FALSE(plugin1 != plugin2);

  EXPECT_TRUE(plugin2 == plugin);
  EXPECT_TRUE(plugin2 == plugin1);
  EXPECT_FALSE(plugin2 != plugin1);

  std::set<ignition::plugin::PluginPtr> orderedSet;
  EXPECT_TRUE(orderedSet.insert(plugin1).second);
  EXPECT_FALSE(orderedSet.insert(plugin1).second);
  EXPECT_FALSE(orderedSet.insert(plugin2).second);

  std::unordered_set<ignition::plugin::PluginPtr> unorderedSet;
  EXPECT_TRUE(unorderedSet.insert(plugin1).second);
  EXPECT_FALSE(unorderedSet.insert(plugin1).second);
  EXPECT_FALSE(unorderedSet.insert(plugin2).second);

  std::map<ignition::plugin::PluginPtr, std::string> orderedMap;
  EXPECT_TRUE(orderedMap.insert(std::make_pair(plugin1, "some string")).second);
  EXPECT_FALSE(orderedMap.insert(std::make_pair(plugin1, "a string")).second);
  EXPECT_FALSE(orderedMap.insert(std::make_pair(plugin2, "chars")).second);

  std::unordered_map<ignition::plugin::PluginPtr, std::string> unorderedMap;
  EXPECT_TRUE(unorderedMap.insert(std::make_pair(plugin1, "strings")).second);
  EXPECT_FALSE(unorderedMap.insert(std::make_pair(plugin1, "letters")).second);
  EXPECT_FALSE(unorderedMap.insert(std::make_pair(plugin2, "")).second);


  plugin2 = loader.Instantiate("test::util::DummyMultiPlugin");
  EXPECT_TRUE(plugin1 != plugin2);
  EXPECT_FALSE(plugin1 == plugin2);
  EXPECT_TRUE(plugin2 != plugin1);
  EXPECT_FALSE(plugin2 == plugin1);

  EXPECT_TRUE(orderedSet.insert(plugin2).second);
  EXPECT_FALSE(orderedSet.insert(plugin2).second);

  EXPECT_TRUE(unorderedSet.insert(plugin2).second);
  EXPECT_FALSE(unorderedSet.insert(plugin2).second);

  EXPECT_TRUE(orderedMap.insert(std::make_pair(plugin2, "letters")).second);
  EXPECT_FALSE(orderedMap.insert(std::make_pair(plugin2, "chars")).second);

  EXPECT_TRUE(unorderedMap.insert(std::make_pair(plugin2, "abc")).second);
  EXPECT_FALSE(unorderedMap.insert(std::make_pair(plugin2, "def")).second);
}

/////////////////////////////////////////////////
using EmptySpecializedPluginPtr =
    ignition::plugin::SpecializedPluginPtr<>;

using SingleSpecializedPluginPtr =
    ignition::plugin::SpecializedPluginPtr<SomeInterface>;

using AnotherSpecializedPluginPtr =
    ignition::plugin::SpecializedPluginPtr<
        SomeInterface,
        test::util::DummyIntBase>;

using DuplicatedInterfaceSpecializedPluginPtr =
    ignition::plugin::SpecializedPluginPtr<
        SomeInterface, SomeInterface>;

/////////////////////////////////////////////////
TEST(PluginPtr, CopyMoveSemantics)
{
  ignition::plugin::PluginPtr plugin;
  EXPECT_TRUE(plugin.IsEmpty());

  ignition::plugin::Loader pl;
  std::string libPath;
  ASSERT_TRUE(IgnDummyPluginLib(libPath));
  pl.LoadLib(libPath);

  plugin = pl.Instantiate("test::util::DummySinglePlugin");
  EXPECT_FALSE(plugin.IsEmpty());

  ignition::plugin::PluginPtr otherPlugin =
      pl.Instantiate("test::util::DummySinglePlugin");
  EXPECT_FALSE(otherPlugin.IsEmpty());

  EXPECT_TRUE(plugin != otherPlugin);
  EXPECT_FALSE(plugin == otherPlugin);

  otherPlugin = plugin;
  EXPECT_TRUE(plugin == otherPlugin);
  EXPECT_FALSE(plugin != otherPlugin);

  TestSetAndMapUsage<
      ignition::plugin::PluginPtr,
      ignition::plugin::PluginPtr>(
        pl, plugin);

  TestSetAndMapUsage<
      ignition::plugin::PluginPtr,
      EmptySpecializedPluginPtr>(
        pl, plugin);

  TestSetAndMapUsage<
      ignition::plugin::PluginPtr,
      SomeSpecializedPluginPtr>(
        pl, plugin);

  TestSetAndMapUsage<
      SomeSpecializedPluginPtr,
      AnotherSpecializedPluginPtr>(
        pl, plugin);

  TestSetAndMapUsage<
      AnotherSpecializedPluginPtr,
      SingleSpecializedPluginPtr>(
        pl, plugin);

  TestSetAndMapUsage<
      DuplicatedInterfaceSpecializedPluginPtr,
      DuplicatedInterfaceSpecializedPluginPtr>(
        pl, plugin);

  ignition::plugin::ConstPluginPtr c_plugin(plugin);
  EXPECT_FALSE(c_plugin.IsEmpty());
  EXPECT_TRUE(c_plugin == plugin);

  c_plugin = nullptr;
  EXPECT_TRUE(c_plugin.IsEmpty());

  c_plugin = otherPlugin;
  EXPECT_FALSE(c_plugin.IsEmpty());
  EXPECT_TRUE(c_plugin == otherPlugin);
}

/////////////////////////////////////////////////
void SetSomeValues(std::shared_ptr<test::util::DummySetterBase> setter)
{
  setter->SetIntegerValue(2468);
  setter->SetDoubleValue(6.28);
  setter->SetName("Changed using shared_ptr");
}

/////////////////////////////////////////////////
void CheckSomeValues(
    std::shared_ptr<test::util::DummyIntBase> getInt,
    std::shared_ptr<test::util::DummyDoubleBase> getDouble,
    std::shared_ptr<test::util::DummyNameBase> getName)
{
  EXPECT_EQ(2468, getInt->MyIntegerValueIs());
  EXPECT_NEAR(6.28, getDouble->MyDoubleValueIs(), 1e-8);
  EXPECT_EQ(std::string("Changed using shared_ptr"), getName->MyNameIs());
}

/////////////////////////////////////////////////
TEST(PluginPtr, QueryInterfaceSharedPtr)
{
  ignition::plugin::Loader pl;
  std::string libPath;
  ASSERT_TRUE(IgnDummyPluginLib(libPath));
  pl.LoadLib(libPath);

  // QueryInterfaceSharedPtr without specialization
  {
    ignition::plugin::PluginPtr plugin =
      pl.Instantiate("test::util::DummyMultiPlugin");

    std::shared_ptr<test::util::DummyIntBase> int_ptr =
      plugin->QueryInterfaceSharedPtr<test::util::DummyIntBase>();
    EXPECT_TRUE(int_ptr.get());
    EXPECT_EQ(5, int_ptr->MyIntegerValueIs());

    std::shared_ptr<const test::util::DummyIntBase> const_int_ptr =
        ignition::plugin::ConstPluginPtr(plugin)
        ->QueryInterfaceSharedPtr<test::util::DummyIntBase>();
    EXPECT_TRUE(const_int_ptr.get());
    EXPECT_EQ(5, int_ptr->MyIntegerValueIs());


    std::shared_ptr<SomeInterface> some_ptr =
      plugin->QueryInterfaceSharedPtr<SomeInterface>();
    EXPECT_FALSE(some_ptr.get());

    std::shared_ptr<const SomeInterface> const_some_ptr =
        ignition::plugin::ConstPluginPtr(plugin)
        ->QueryInterfaceSharedPtr<SomeInterface>();
    EXPECT_FALSE(const_some_ptr.get());
  }

  std::shared_ptr<test::util::DummyIntBase> int_ptr =
      pl.Instantiate("test::util::DummyMultiPlugin")->
        QueryInterfaceSharedPtr<test::util::DummyIntBase>();
  EXPECT_TRUE(int_ptr.get());
  EXPECT_EQ(5, int_ptr->MyIntegerValueIs());

  SomeSpecializedPluginPtr plugin =
      pl.Instantiate("test::util::DummyMultiPlugin");


  usedSpecializedInterfaceAccess = false;
  std::shared_ptr<test::util::DummySetterBase> setter =
      plugin->QueryInterfaceSharedPtr<test::util::DummySetterBase>();
  EXPECT_TRUE(usedSpecializedInterfaceAccess);
  ASSERT_TRUE(setter.get());

  usedSpecializedInterfaceAccess = false;
  std::shared_ptr<SomeInterface> someInterface =
      plugin->QueryInterfaceSharedPtr<SomeInterface>();
  EXPECT_TRUE(usedSpecializedInterfaceAccess);
  EXPECT_FALSE(someInterface.get());

  usedSpecializedInterfaceAccess = false;
  std::shared_ptr<test::util::DummyIntBase> getInt =
      plugin->QueryInterfaceSharedPtr<test::util::DummyIntBase>();
  EXPECT_TRUE(usedSpecializedInterfaceAccess);
  ASSERT_TRUE(getInt.get());

  std::shared_ptr<test::util::DummyDoubleBase> getDouble =
      plugin->QueryInterfaceSharedPtr<test::util::DummyDoubleBase>();
  ASSERT_TRUE(getDouble.get());

  std::shared_ptr<test::util::DummyNameBase> getName =
      plugin->QueryInterfaceSharedPtr<test::util::DummyNameBase>();
  ASSERT_TRUE(getName.get());

  SetSomeValues(setter);
  CheckSomeValues(getInt, getDouble, getName);
}

/////////////////////////////////////////////////
ignition::plugin::PluginPtr GetSomePlugin(const std::string &path)
{
  ignition::plugin::Loader pl;
  pl.LoadLib(path);

  return pl.Instantiate("test::util::DummyMultiPlugin");
}

/////////////////////////////////////////////////
TEST(PluginPtr, LibraryManagement)
{
  std::string libPath;
  ASSERT_TRUE(IgnDummyPluginLib(libPath));
  const std::string &path = libPath;

  // Use scoping to destroy somePlugin
  {
    ignition::plugin::PluginPtr somePlugin = GetSomePlugin(path);
    EXPECT_TRUE(somePlugin);

    CHECK_FOR_LIBRARY(path, true);
  }

  CHECK_FOR_LIBRARY(path, false);

  // Test that we can transfer between plugins
  {
    ignition::plugin::PluginPtr somePlugin;
    CHECK_FOR_LIBRARY(path, false);

    {
      ignition::plugin::PluginPtr temporaryPlugin = GetSomePlugin(path);
      CHECK_FOR_LIBRARY(path, true);
      somePlugin = temporaryPlugin;
    }

    CHECK_FOR_LIBRARY(path, true);
  }

  CHECK_FOR_LIBRARY(path, false);

  // Test that we can forget libraries
  {
    ignition::plugin::Loader pl;
    pl.LoadLib(path);

    CHECK_FOR_LIBRARY(path, true);

    EXPECT_TRUE(pl.ForgetLibrary(path));

    CHECK_FOR_LIBRARY(path, false);
  }

  // Test that we can forget libraries, but the library will remain loaded if
  // a plugin instance is still using it.
  {
    ignition::plugin::PluginPtr plugin;

    ignition::plugin::Loader pl;
    pl.LoadLib(path);

    CHECK_FOR_LIBRARY(path, true);

    plugin = pl.Instantiate("test::util::DummyMultiPlugin");
    EXPECT_TRUE(plugin);

    EXPECT_TRUE(pl.ForgetLibrary(path));

    CHECK_FOR_LIBRARY(path, true);
  }

  // Check that the library will be unloaded once the plugin instance is deleted
  CHECK_FOR_LIBRARY(path, false);

  // Check that we can unload libraries based on plugin name
  {
    ignition::plugin::Loader pl;
    pl.LoadLib(path);

    CHECK_FOR_LIBRARY(path, true);

    pl.ForgetLibraryOfPlugin("test::util::DummyMultiPlugin");

    CHECK_FOR_LIBRARY(path, false);
  }

  // Check that the std::shared_ptrs that we provide for interfaces will
  // successfully keep the library loaded.
  {
    std::shared_ptr<test::util::DummyNameBase> interface;

    CHECK_FOR_LIBRARY(path, false);
    {
      interface = GetSomePlugin(path)->QueryInterfaceSharedPtr<
          test::util::DummyNameBase>();
      EXPECT_EQ("DummyMultiPlugin", interface->MyNameIs());

      CHECK_FOR_LIBRARY(path, true);
    }

    EXPECT_EQ("DummyMultiPlugin", interface->MyNameIs());

    CHECK_FOR_LIBRARY(path, true);
  }

  CHECK_FOR_LIBRARY(path, false);

  // Check that mulitple Loaders can work side-by-side
  {
    ignition::plugin::Loader pl1;
    pl1.LoadLib(path);

    CHECK_FOR_LIBRARY(path, true);

    {
      ignition::plugin::Loader pl2;
      pl2.LoadLib(path);

      CHECK_FOR_LIBRARY(path, true);
    }

    CHECK_FOR_LIBRARY(path, true);
  }

  CHECK_FOR_LIBRARY(path, false);
}

/////////////////////////////////////////////////
int main(int argc, char **argv)
{
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}
