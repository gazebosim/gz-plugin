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
#include <gz/utils/cli/CLI.hpp>

#include "ign.hh"

//////////////////////////////////////////////////
/// \brief Enumeration of available commands
enum class PluginCommand
{
  kNone,
  kPluginInfo
};

//////////////////////////////////////////////////
/// \brief Structure to hold all available topic options
struct PluginOptions
{
  /// \brief Command to execute
  PluginCommand command{PluginCommand::kNone};

  int verboseLevel = 0;

  std::string pluginName;
};

//////////////////////////////////////////////////
void runPluginCommand(const PluginOptions &_opt)
{
  if (_opt.command == PluginCommand::kPluginInfo)
  {
    cmdPluginInfo(_opt.pluginName.c_str(), _opt.verboseLevel);
  }
  else if (_opt.command == PluginCommand::kNone)
  {
    // In the event that there is no command, display help
    throw CLI::CallForHelp();
  }
}

void addPluginFlags(CLI::App &_app)
{
  auto opt = std::make_shared<PluginOptions>();

  _app.add_flag_callback("-v,--verbose",
    [opt](){
      opt->verboseLevel = 1;
    }, "Print verbose info.");

  auto info = _app.add_flag_callback("-i,--info",
     [opt](){
       opt->command = PluginCommand::kPluginInfo;
     }, "Get info about a plugin.\nRequires the -p option.");

 _app.add_option("-p,--plugin",
                  opt->pluginName,
                  "Path to a plugin.\n"
                  "Required with -i.")->needs(info);

  _app.callback([&_app, opt](){
    runPluginCommand(*opt);
  });
}

//////////////////////////////////////////////////
int main(int argc, char** argv)
{
  CLI::App app{"Introspect Ignition plugin"};

  app.set_help_all_flag("--help-all", "Show all help");

  app.add_flag_callback("--version", [](){
      std::cout << ignitionVersion() << std::endl;
      throw CLI::Success();
  });

  addPluginFlags(app);
  CLI11_PARSE(app, argc, argv);
}
