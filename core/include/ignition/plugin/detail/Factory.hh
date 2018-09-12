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

#ifndef IGNITION_PLUGIN_DETAIL_FACTORY_HH_
#define IGNITION_PLUGIN_DETAIL_FACTORY_HH_

#include <memory>
#include <utility>

#include <ignition/plugin/Factory.hh>

namespace ignition
{
  namespace plugin
  {
    template <typename Interface>
    class Deleter
    {
      public: Deleter()
        : pluginInstancePtr(nullptr)
      {
        // Do nothing
      }

      public: Deleter(std::shared_ptr<void> _pluginInstancePtr)
        : pluginInstancePtr(
            new std::shared_ptr<void>(std::move(_pluginInstancePtr)))
      {
        // Do nothing
      }

      public: void operator()(Interface *_ptr)
      {
        delete _ptr;

        // Release the reference to the plugin
        if (pluginInstancePtr)
          *pluginInstancePtr = nullptr;
      }

      public: ~Deleter()
      {
        if (pluginInstancePtr && *pluginInstancePtr)
        {
          // If this std::shared_ptr still contains a value, then that means
          // the original object has been released from its ownership and will
          // never be able to get deleted by this deleter. Therefore, we should
          // release the pluginInstancePtr so that the library remains loaded.
          //
          // Note that this is an intentional memory leak to accommodate the use
          // case where a user simply cannot retain ownership of the object from
          // the plugin and is okay with its library remaining loaded forever.
        }
        else if (pluginInstancePtr)
        {
          delete pluginInstancePtr;
        }
      }

      private: std::shared_ptr<void> *pluginInstancePtr;
    };

    template <typename Interface, typename... Args>
    auto Factory<Interface, Args...>::Construct(Args&&... _args) -> InterfacePtr
    {
      return InterfacePtr(
            this->ImplConstruct(std::forward<Args>(_args)...),
            Deleter<Interface>(this->PluginInstancePtrFromThis()));
    }

    /// \brief Producing provides the implementation of Factory for a specific
    /// derivative of Factory's Interface type. That derivative is called
    /// Product, which must be a fully-defined class that implements Interface.
    ///
    /// The mechanism of this class ensures that as long as the Factory output
    /// object is alive, the plugin library that it depends on will remain
    /// safely loaded.
    template <typename Interface, typename... Args>
    // cppcheck-suppress syntaxError
    template <typename Product>
    class Factory<Interface, Args...>::Producing
        : public Factory<Interface, Args...>
    {
      private: Interface *ImplConstruct(Args&&... _args) override
      {
        return new Product(std::forward<Args>(_args)...);
      }
    };
  }
}

#endif
