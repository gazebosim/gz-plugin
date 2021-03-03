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

#include <ignition/plugin/Loader.hh>
#include <ignition/plugin/Register.hh>

#include "DummyPlugins.hh"
#include "LoadsAnotherPlugin.hh"

namespace test
{
namespace util
{

class LoadsAnotherPlugin : public LoadsAnotherPluginInterface
{
  public: virtual std::unordered_set<std::string> Load(int _flags)
      override
  {
    if (_flags >= 0)
    {
      this->loader.SetFlags(_flags);
    }

    auto pluginNames = this->loader.LoadLib(IGNDummyPlugins_LIB);
    return pluginNames;
  }

  public: virtual bool Unload() override
  {
    return this->loader.ForgetLibrary(IGNDummyPlugins_LIB);
  }

  private: ignition::plugin::Loader loader;
};

}
}

IGNITION_ADD_PLUGIN(test::util::LoadsAnotherPlugin,
    test::util::LoadsAnotherPluginInterface)
