/*
 * Copyright (C) 2021 Open Source Robotics Foundation
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

#include <dlfcn.h>
#include <gtest/gtest.h>

#include <ignition/plugin/Loader.hh>

#include "../plugins/LoadsAnotherPlugin.hh"

/////////////////////////////////////////////////
TEST(Flags, DefaultFlags)
{
  ignition::plugin::Loader loader;
  EXPECT_EQ(0u, loader.InterfacesImplemented().size());

  // Load plugin
  auto loadedByTest = loader.LoadLib(IGNLoadsAnotherPlugin_LIB);
  EXPECT_EQ(1u, loader.InterfacesImplemented().size());
  EXPECT_EQ(1u, loadedByTest.size());
  ASSERT_EQ(1u, loadedByTest.count("test::util::LoadsAnotherPlugin"));

  auto plugin = loader.Instantiate(*loadedByTest.begin());
  EXPECT_FALSE(plugin.IsEmpty());

  auto interface = plugin->QueryInterface<
      test::util::LoadsAnotherPluginInterface>();
  ASSERT_NE(nullptr, interface);

  // Load other plugins from this plugin
  auto loadedByPlugin = interface->Load();
  EXPECT_EQ(3u, loadedByPlugin.size());
  ASSERT_EQ(1u, loadedByPlugin.count("test::util::DummySinglePlugin"));
  ASSERT_EQ(1u, loadedByPlugin.count("test::util::DummyMultiPlugin"));
  ASSERT_EQ(1u, loadedByPlugin.count("test::util::DummyNoAliasPlugin"));

  // Load another plugin
  loadedByTest = loader.LoadLib(IGNTemplatedPlugins_LIB);
  EXPECT_EQ(5u, loader.InterfacesImplemented().size());
  EXPECT_EQ(4u, loadedByTest.size());
  ASSERT_EQ(1u, loadedByTest.count(
      "test::plugins::GenericTemplatePlugin<std::__cxx11::basic_string"
      "<char, std::char_traits<char>, std::allocator<char> > >"));
  ASSERT_EQ(1u, loadedByTest.count(
      "test::plugins::GenericTemplatePlugin<int>"));
  ASSERT_EQ(1u, loadedByTest.count("test::plugins::StringTemplatePlugin"));
  ASSERT_EQ(1u, loadedByTest.count("test::plugins::DoubleTemplatePlugin"));

  // Forget
  EXPECT_TRUE(interface->Unload());
  EXPECT_TRUE(loader.ForgetLibrary(IGNLoadsAnotherPlugin_LIB));
  EXPECT_TRUE(loader.ForgetLibrary(IGNTemplatedPlugins_LIB));
  EXPECT_EQ(0u, loader.InterfacesImplemented().size());
}

/////////////////////////////////////////////////
TEST(Flags, ChildPluginsGlobal)
{
  ignition::plugin::Loader loader;
  EXPECT_EQ(0u, loader.InterfacesImplemented().size());

  // Load plugin
  auto loadedByTest = loader.LoadLib(IGNLoadsAnotherPlugin_LIB);
  EXPECT_EQ(1u, loader.InterfacesImplemented().size());
  EXPECT_EQ(1u, loadedByTest.size());
  ASSERT_EQ(1u, loadedByTest.count("test::util::LoadsAnotherPlugin"));

  auto plugin = loader.Instantiate(*loadedByTest.begin());
  EXPECT_FALSE(plugin.IsEmpty());

  auto interface = plugin->QueryInterface<
      test::util::LoadsAnotherPluginInterface>();
  ASSERT_NE(nullptr, interface);

  // Load other plugins from this plugin, globally
  auto loadedByPlugin = interface->Load(RTLD_LAZY | RTLD_GLOBAL);
  EXPECT_EQ(3u, loadedByPlugin.size());
  ASSERT_EQ(1u, loadedByPlugin.count("test::util::DummySinglePlugin"));
  ASSERT_EQ(1u, loadedByPlugin.count("test::util::DummyMultiPlugin"));
  ASSERT_EQ(1u, loadedByPlugin.count("test::util::DummyNoAliasPlugin"));

  // Load another plugin
  loadedByTest = loader.LoadLib(IGNTemplatedPlugins_LIB);
  EXPECT_EQ(12u, loader.InterfacesImplemented().size());
  EXPECT_EQ(7u, loadedByTest.size());
  ASSERT_EQ(1u, loadedByTest.count(
      "test::plugins::GenericTemplatePlugin<std::__cxx11::basic_string"
      "<char, std::char_traits<char>, std::allocator<char> > >"));
  ASSERT_EQ(1u, loadedByTest.count(
      "test::plugins::GenericTemplatePlugin<int>"));
  ASSERT_EQ(1u, loadedByTest.count("test::plugins::StringTemplatePlugin"));
  ASSERT_EQ(1u, loadedByTest.count("test::plugins::DoubleTemplatePlugin"));
  ASSERT_EQ(1u, loadedByTest.count("test::util::DummySinglePlugin"));
  ASSERT_EQ(1u, loadedByTest.count("test::util::DummyMultiPlugin"));
  ASSERT_EQ(1u, loadedByTest.count("test::util::DummyNoAliasPlugin"));

  // Forget
  EXPECT_TRUE(interface->Unload());
  EXPECT_TRUE(loader.ForgetLibrary(IGNLoadsAnotherPlugin_LIB));
  EXPECT_TRUE(loader.ForgetLibrary(IGNTemplatedPlugins_LIB));
  EXPECT_EQ(0u, loader.InterfacesImplemented().size());
}

/////////////////////////////////////////////////
TEST(Flags, ParentPluginsGlobal)
{
  // ignition::plugin::Loader loader;
  // loader.SetFlags(RTLD_LAZY | RTLD_GLOBAL);
  // EXPECT_EQ(0u, loader.InterfacesImplemented().size());

  // // Load plugin
  // auto loadedByTest = loader.LoadLib(IGNLoadsAnotherPlugin_LIB);
  // EXPECT_EQ(1u, loader.InterfacesImplemented().size());
  // EXPECT_EQ(1u, loadedByTest.size());
  // ASSERT_EQ(1u, loadedByTest.count("test::util::LoadsAnotherPlugin"));

  // auto plugin = loader.Instantiate(*loadedByTest.begin());
  // EXPECT_FALSE(plugin.IsEmpty());

  // auto interface = plugin->QueryInterface<
  //     test::util::LoadsAnotherPluginInterface>();
  // ASSERT_NE(nullptr, interface);

  // // Load other plugins from this plugin, it gets the global plugin too
  // auto loadedByPlugin = interface->Load();
  // EXPECT_EQ(4u, loadedByPlugin.size());
  // ASSERT_EQ(1u, loadedByPlugin.count("test::util::DummySinglePlugin"));
  // ASSERT_EQ(1u, loadedByPlugin.count("test::util::DummyMultiPlugin"));
  // ASSERT_EQ(1u, loadedByPlugin.count("test::util::DummyNoAliasPlugin"));
  // ASSERT_EQ(1u, loadedByPlugin.count("test::util::LoadsAnotherPlugin"));

  // // Load another plugin
  // loadedByTest = loader.LoadLib(IGNTemplatedPlugins_LIB);
  // EXPECT_EQ(5u, loader.InterfacesImplemented().size());
  // EXPECT_EQ(4u, loadedByTest.size());
  // ASSERT_EQ(1u, loadedByTest.count(
  //     "test::plugins::GenericTemplatePlugin<std::__cxx11::basic_string"
  //     "<char, std::char_traits<char>, std::allocator<char> > >"));
  // ASSERT_EQ(1u, loadedByTest.count(
  //     "test::plugins::GenericTemplatePlugin<int>"));
  // ASSERT_EQ(1u, loadedByTest.count("test::plugins::StringTemplatePlugin"));
  // ASSERT_EQ(1u, loadedByTest.count("test::plugins::DoubleTemplatePlugin"));

  // // Forget
  // EXPECT_TRUE(interface->Unload());
  // EXPECT_TRUE(loader.ForgetLibrary(IGNLoadsAnotherPlugin_LIB));
  // // EXPECT_TRUE(loader.ForgetLibrary(IGNTemplatedPlugins_LIB));
  // EXPECT_EQ(0u, loader.InterfacesImplemented().size());
}

/////////////////////////////////////////////////
TEST(Flags, AllPluginsGlobal)
{
  ignition::plugin::Loader loader;
  loader.SetFlags(RTLD_LAZY | RTLD_GLOBAL);
  EXPECT_EQ(0u, loader.InterfacesImplemented().size());

  // // Load plugin
  // auto loadedByTest = loader.LoadLib(IGNLoadsAnotherPlugin_LIB);
  // EXPECT_EQ(1u, loader.InterfacesImplemented().size());
  // EXPECT_EQ(1u, loadedByTest.size());
  // ASSERT_EQ(1u, loadedByTest.count("test::util::LoadsAnotherPlugin"));

  // auto plugin = loader.Instantiate(*loadedByTest.begin());
  // EXPECT_FALSE(plugin.IsEmpty());

  // auto interface = plugin->QueryInterface<
  //     test::util::LoadsAnotherPluginInterface>();
  // ASSERT_NE(nullptr, interface);

  // // Load other plugins from this plugin, it gets the global plugin too
  // auto loadedByPlugin = interface->Load(RTLD_LAZY | RTLD_GLOBAL);
  // EXPECT_EQ(4u, loadedByPlugin.size());
  // ASSERT_EQ(1u, loadedByPlugin.count("test::util::DummySinglePlugin"));
  // ASSERT_EQ(1u, loadedByPlugin.count("test::util::DummyMultiPlugin"));
  // ASSERT_EQ(1u, loadedByPlugin.count("test::util::DummyNoAliasPlugin"));
  // ASSERT_EQ(1u, loadedByPlugin.count("test::util::LoadsAnotherPlugin"));

  // // Load another plugin
  // loadedByTest = loader.LoadLib(IGNTemplatedPlugins_LIB);
  // EXPECT_EQ(5u, loader.InterfacesImplemented().size());
  // EXPECT_EQ(4u, loadedByTest.size());
  // ASSERT_EQ(1u, loadedByTest.count(
  //     "test::plugins::GenericTemplatePlugin<std::__cxx11::basic_string"
  //     "<char, std::char_traits<char>, std::allocator<char> > >"));
  // ASSERT_EQ(1u, loadedByTest.count(
  //     "test::plugins::GenericTemplatePlugin<int>"));
  // ASSERT_EQ(1u, loadedByTest.count("test::plugins::StringTemplatePlugin"));
  // ASSERT_EQ(1u, loadedByTest.count("test::plugins::DoubleTemplatePlugin"));

  // // Forget
  // EXPECT_TRUE(interface->Unload());
  // EXPECT_TRUE(loader.ForgetLibrary(IGNLoadsAnotherPlugin_LIB));
  // // EXPECT_TRUE(loader.ForgetLibrary(IGNTemplatedPlugins_LIB));
  // EXPECT_EQ(0u, loader.InterfacesImplemented().size());
}

/////////////////////////////////////////////////
int main(int argc, char **argv)
{
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}
