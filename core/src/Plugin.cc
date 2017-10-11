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


#include "ignition/common/Plugin.hh"
#include "ignition/common/PluginInfo.hh"
#include "ignition/common/Console.hh"
#include "PluginUtils.hh"

namespace ignition
{
  namespace common
  {
    class PluginPrivate
    {
      /// \brief Clear this PluginPrivate without invaliding any map entry
      /// iterators.
      public: void Clear()
      {
        this->loadedInstancePtr.reset();

        // Dev note (MXG): We must NOT call clear() on the InterfaceMap or
        // remove ANY of the map entries, because that would potentially
        // invalidate all of the iterators that are pointing to map entries.
        // This would break any specialized plugins that provide instant access
        // to specialized interfaces. Instead, we simply overwrite the map
        // entries with a nullptr.
        for (auto &entry : this->interfaces)
          entry.second = nullptr;
      }

      /// \brief Initialize this PluginPrivate using some PluginInfo instance
      /// \param[in] _info Information describing the plugin to initialize
      public: void Initialize(const PluginInfo *_info)
      {
        this->Clear();

        if (!_info)
          return;

        this->loadedInstancePtr =
            std::shared_ptr<void>(_info->factory(), _info->deleter);

        if (this->loadedInstancePtr)
        {
          for (const auto &entry : _info->interfaces)
          {
            // entry.first:  name of the interface
            // entry.second: function which casts the pluginInstance pointer to
            //               the correct location of the interface within the
            //               plugin
            this->interfaces[entry.first] =
                entry.second(this->loadedInstancePtr.get());
          }
        }
      }

      /// \brief Initialize this PluginPrivate using another instance
      /// \param[in] _other Another instance of a PluginPrivate object
      public: void Initialize(const PluginPrivate *_other)
      {
        this->Clear();

        if (!_other)
        {
          ignerr << "Received a nullptr _other in the constructor "
                 << "which uses `const PluginPrivate*`. This should "
                 << "not be possible! Please report this bug."
                 << std::endl;
          assert(false);
          return;
        }

        this->loadedInstancePtr = _other->loadedInstancePtr;

        if (this->loadedInstancePtr)
        {
          for (const auto &entry : _other->interfaces)
          {
            // entry.first:  name of the interface
            // entry.second: pointer to the location of that interface within
            //               the plugin instance
            this->interfaces[entry.first] = entry.second;
          }
        }
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

      /// \brief shared_ptr which manages the lifecycle of the plugin instance.
      std::shared_ptr<void> loadedInstancePtr;
    };

    //////////////////////////////////////////////////
    bool Plugin::HasInterface(
        const std::string &_interfaceName) const
    {
      const std::string interfaceName = NormalizeName(_interfaceName);
      return (this->dataPtr->interfaces.count(interfaceName) != 0);
    }

    //////////////////////////////////////////////////
    Plugin::Plugin()
      : dataPtr(new PluginPrivate)
    {
      // Do nothing
    }

    //////////////////////////////////////////////////
    void *Plugin::PrivateGetInterface(
        const std::string &_interfaceName) const
    {
      const std::string interfaceName = NormalizeName(_interfaceName);
      const auto &it = this->dataPtr->interfaces.find(interfaceName);
      if (this->dataPtr->interfaces.end() == it)
        return nullptr;

      return it->second;
    }

    //////////////////////////////////////////////////
    void Plugin::PrivateCopyPluginInstance(const Plugin &_other) const
    {
      this->dataPtr->Initialize(_other.dataPtr.get());
    }

    //////////////////////////////////////////////////
    void Plugin::PrivateSetPluginInstance(const PluginInfo *_info) const
    {
      this->dataPtr->Initialize(_info);
    }

    //////////////////////////////////////////////////
    const std::shared_ptr<void> &Plugin::PrivateGetInstancePtr() const
    {
      return this->dataPtr->loadedInstancePtr;
    }

    //////////////////////////////////////////////////
    Plugin::InterfaceMap::iterator Plugin::PrivateGetOrCreateIterator(
        const std::string &_interfaceName)
    {
      // We want to use the insert function here to avoid accidentally
      // overwriting a value which might exist at the desired map key.
      return this->dataPtr->interfaces.insert(
            std::make_pair(NormalizeName(_interfaceName), nullptr)).first;
    }

    //////////////////////////////////////////////////
    Plugin::~Plugin()
    {
      // Do nothing. We need this definition to be in a source file so that
      // the destructor of PluginPrivate is visible to std::unique_ptr.
    }
  }
}
