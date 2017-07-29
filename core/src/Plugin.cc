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

#include <map>

#include "ignition/common/Plugin.hh"
#include "ignition/common/PluginInfo.hh"
#include "PluginUtils.hh"

namespace ignition
{
  namespace common
  {
    class PluginPrivate
    {
      public: PluginPrivate()
                : pluginInstance(nullptr)
              {
                // Do nothing
              }

      /// \brief Map from interface names to their locations within the plugin
      /// instance
      //
      // Dev Note (MXG): We use std::map here instead of std::unordered_map
      // because iterators to a std::map are not invalidated by the insertion
      // operation (whereas all iterators to a std::unordered_map are
      // potentially invalidated each time an insertion is performed on the
      // std::unordered_map). Holding onto valid iterators allows us to do
      // optimizations with template magic to provide direct access to
      // interfaces whose availability we can anticipate at run time.
      //
      // It is also worth noting that ordered vs unordered lookup time is very
      // similar on sets where the strings have relatively small lengths (5-20
      // characters) and a relatively small number of entries in the set (5-20
      // entries). Those conditions match our expected use case here. In fact,
      // ordered lookup can sometimes outperform unordered in these conditions.
      public: Plugin::InterfaceMap interfaces;

      /// \brief ptr which manages the lifecycle of the plugin instance. Note
      /// that we cannot use a std::unique_ptr<void> here because unique_ptr
      /// is statically constrained to only hold onto a complete type, and
      /// "void" never qualifies as a complete type.
      public: void *pluginInstance;

      /// \brief Function which can be invoked to delete our plugin instance
      public: std::function<void(void*)> pluginDeleter;

      /// \brief Destructor
      public: ~PluginPrivate()
      {
        // When the Plugin object is deleted, we must delete our pluginInstance
        // using the deleter that was provided by the external library.
        if(pluginInstance)
          pluginDeleter(pluginInstance);
      }
    };

    bool Plugin::HasInterface(const std::string &_interfaceName) const
    {
      const std::string interfaceName = NormalizeName(_interfaceName);
      return (this->dataPtr->interfaces.count(interfaceName) != 0);
    }

    Plugin::~Plugin()
    {
      delete dataPtr;
    }

    void *Plugin::PrivateGetInterface(const std::string &_interfaceName) const
    {
      const std::string interfaceName = NormalizeName(_interfaceName);
      const auto &it = this->dataPtr->interfaces.find(interfaceName);
      if(this->dataPtr->interfaces.end() == it)
        return nullptr;

      return it->second;
    }

    Plugin::Plugin(const PluginInfo *info)
      : dataPtr(new PluginPrivate)
    {
      if(info)
      {
        // Create a new instance of the plugin, and store it in our dataPtr
        this->dataPtr->pluginInstance = info->factory();

        // Grab a copy of the plugin's deleter
        this->dataPtr->pluginDeleter = info->deleter;

        void * const instance = this->dataPtr->pluginInstance;
        // For each interface provided by the plugin, insert its location within
        // the instance into the map
        for(const auto &entry : info->interfaces)
        {
          // entry.first: name of the interface
          // entry.second: function which casts the pluginInstance pointer to the
          //               correct location of the interface within the plugin
          this->dataPtr->interfaces.insert(
                std::make_pair(entry.first, entry.second(instance)));
        }
      }
    }

    Plugin::InterfaceMap::iterator Plugin::PrivateGetOrCreateIterator(
        const std::string &_interfaceName)
    {
      return this->dataPtr->interfaces.insert(
            std::make_pair(_interfaceName, nullptr)).first;
    }
  }
}
