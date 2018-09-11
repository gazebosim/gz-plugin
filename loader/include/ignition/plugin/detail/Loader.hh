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


#ifndef IGNITION_PLUGIN_DETAIL_LOADER_HH_
#define IGNITION_PLUGIN_DETAIL_LOADER_HH_

#include <memory>
#include <string>
#include <unordered_set>
#include <ignition/plugin/EnablePluginFromThis.hh>
#include <ignition/plugin/Loader.hh>

namespace ignition
{
  namespace plugin
  {
    template <typename Interface>
    std::unordered_set<std::string> Loader::PluginsImplementing() const
    {
      return this->PluginsImplementing(typeid(Interface).name(), false);
    }

    template <typename PluginPtrType>
    PluginPtrType Loader::Instantiate(
        const std::string &_pluginNameOrAlias) const
    {
      const std::string &resolvedName = this->LookupPlugin(_pluginNameOrAlias);
      if (resolvedName.empty())
        return PluginPtr();

       PluginPtrType ptr(this->PrivateGetInfo(resolvedName),
                         this->PrivateGetPluginDlHandlePtr(resolvedName));

       if (auto *enableFromThis =
              ptr->template QueryInterface<EnablePluginFromThis>())
         enableFromThis->PrivateSetPluginFromThis(ptr);

       return ptr;
    }

    template <typename InterfaceType>
    std::shared_ptr<InterfaceType> Loader::Factory(
        const std::string &_pluginNameOrAlias) const
    {
      return this->Instantiate(_pluginNameOrAlias)
          ->template QueryInterfaceSharedPtr<InterfaceType>();
    }
  }
}

#endif
