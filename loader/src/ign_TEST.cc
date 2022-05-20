/*
 * Copyright (C) 2020 Open Source Robotics Foundation
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

#include <iostream>
#include <string>

#include "gtest/gtest.h"
#include "gz/plugin/Loader.hh"

#ifdef _MSC_VER
#    define popen _popen
#    define pclose _pclose
#endif

using namespace gz;

static const std::string g_ignVersion("--force-version " + // NOLINT(*)
  std::string(IGN_VERSION_FULL));

/////////////////////////////////////////////////
std::string custom_exec_str(std::string _cmd)
{
  _cmd += " 2>&1";
  FILE *pipe = popen(_cmd.c_str(), "r");

  if (!pipe)
    return "ERROR";

  char buffer[128];
  std::string result = "";

  while (!feof(pipe))
  {
    if (fgets(buffer, 128, pipe) != NULL)
      result += buffer;
  }

  pclose(pipe);
  return result;
}

//////////////////////////////////////////////////
/// \brief Check 'ign plugin --help'.
TEST(ignTest, IgnPluginHelp)
{
  // Path to ign executable
  std::string ign = std::string(IGN_PATH);
  std::string output = custom_exec_str(ign + " plugin --help");
  EXPECT_NE(std::string::npos,
    output.find("-i,--info                   Get info about a plugin."))
      << output;
  EXPECT_NE(std::string::npos,
    output.find("-p,--plugin TEXT Needs: --info"))
      << output;

  output = custom_exec_str(ign + " plugin");
  EXPECT_NE(std::string::npos,
    output.find("-i,--info                   Get info about a plugin."))
      << output;
  EXPECT_NE(std::string::npos,
    output.find("-p,--plugin TEXT Needs: --info"))
      << output;
}

//////////////////////////////////////////////////
/// \brief Check 'ign plugin --info' for a non-existent file.
TEST(ignTest, PluginInfoNonexistentLibrary)
{
  // Path to ign executable
  std::string ign = std::string(IGN_PATH);

  std::string output = custom_exec_str(ign + " plugin --info --plugin " +
      "/path/to/libDoesNotExist.so");

  EXPECT_NE(std::string::npos, output.find("Error while loading the library"))
      << output;
  EXPECT_NE(std::string::npos, output.find("Found 0 plugins in library file"))
      << output;
  EXPECT_NE(std::string::npos, output.find("Found 0 interfaces in library"))
      << output;
}

//////////////////////////////////////////////////
/// \brief Check 'ign plugin --info' for a file that isn't a shared library.
TEST(ignTest, PluginInfoNonLibrary)
{
  // Path to ign executable
  std::string ign = std::string(IGN_PATH);

  std::string output = custom_exec_str(ign + " plugin --info --plugin " +
      std::string(IGN_PLUGIN_SOURCE_DIR) + "/core/src/Plugin.cc");

  EXPECT_NE(std::string::npos, output.find("Error while loading the library"))
      << output;
  EXPECT_NE(std::string::npos, output.find("Found 0 plugins in library file"))
      << output;
  EXPECT_NE(std::string::npos, output.find("Found 0 interfaces in library"))
      << output;
}

//////////////////////////////////////////////////
/// \brief Check 'ign plugin --info' for a library that has no plugins.
TEST(ignTest, PluginInfoNonPluginLibrary)
{
  // Path to ign executable
  std::string ign = std::string(IGN_PATH);

  std::string output = custom_exec_str(ign + " plugin --info --plugin " +
      IGN_PLUGIN_LIB);

  EXPECT_NE(std::string::npos, output.find("does not export any plugins. The "
    "symbol [IgnitionPluginHook] is missing, or it is not externally visible."))
      << output;
  EXPECT_NE(std::string::npos, output.find("Found 0 plugins in library file"))
      << output;
  EXPECT_NE(std::string::npos, output.find("Found 0 interfaces in library"))
      << output;
}

//////////////////////////////////////////////////
/// \brief Check 'ign plugin --info' for a library with plugins.
TEST(ignTest, PluginInfoDummyPlugins)
{
  // Path to ign executable
  std::string ign = std::string(IGN_PATH);

  std::string output = custom_exec_str(ign + " plugin --info --plugin " +
      IGNDummyPlugins_LIB);

  EXPECT_NE(std::string::npos, output.find("Found 3 plugins in library file"))
      << output;
  EXPECT_NE(std::string::npos, output.find("test::util::DummyMultiPlugin"))
      << output;
  EXPECT_NE(std::string::npos, output.find("test::util::DummyNoAliasPlugin"))
      << output;
  EXPECT_NE(std::string::npos, output.find("test::util::DummySinglePlugin"))
      << output;

  EXPECT_NE(std::string::npos, output.find("Found 7 interfaces in library"))
      << output;
  EXPECT_NE(std::string::npos, output.find("test::util::DummyNameBase"))
      << output;
  EXPECT_NE(std::string::npos, output.find("test::util::DummyGetObjectBase"))
      << output;
  EXPECT_NE(std::string::npos, output.find("test::util::DummyIntBase"))
      << output;
  EXPECT_NE(std::string::npos,
            output.find("test::util::DummyGetPluginInstancePtr"))
      << output;
  EXPECT_NE(std::string::npos, output.find("test::util::DummyDoubleBase"))
      << output;
  EXPECT_NE(std::string::npos, output.find("test::util::DummySetterBase"))
      << output;
  EXPECT_NE(std::string::npos,
            output.find("gz::plugin::EnablePluginFromThis"))
      << output;
}

//////////////////////////////////////////////////
/// \brief Check 'ign plugin --info' with verbose output.
TEST(ignTest, PluginInfoVerboseDummyPlugins)
{
  // Path to ign executable
  std::string ign = std::string(IGN_PATH);

  std::string output = custom_exec_str(ign + " plugin --info --plugin " +
      IGNDummyPlugins_LIB + " --verbose");

  EXPECT_NE(std::string::npos, output.find("Known Interfaces: 7"))
      << output;
  EXPECT_NE(std::string::npos, output.find("test::util::DummyNameBase"))
      << output;
  EXPECT_NE(std::string::npos, output.find("test::util::DummyGetObjectBase"))
      << output;
  EXPECT_NE(std::string::npos, output.find("test::util::DummyIntBase"))
      << output;
  EXPECT_NE(std::string::npos,
            output.find("test::util::DummyGetPluginInstancePtr"))
      << output;
  EXPECT_NE(std::string::npos, output.find("test::util::DummyDoubleBase"))
      << output;
  EXPECT_NE(std::string::npos, output.find("test::util::DummySetterBase"))
      << output;
  EXPECT_NE(std::string::npos,
            output.find("gz::plugin::EnablePluginFromThis"))
      << output;

  EXPECT_NE(std::string::npos, output.find("Known Plugins: 3"))
      << output;

  EXPECT_NE(std::string::npos,
            output.find("There are 2 aliases with a name collision"))
      << output;
}
