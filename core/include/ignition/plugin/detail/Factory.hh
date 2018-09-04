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

#include <ignition/plugin/Factory.hh>

namespace ignition
{
  namespace plugin
  {
    template <typename Interface, typename... Args>
    template <typename Product>
    class Factory<Interface, Args...>::Producing
        : public Factory<Interface, Args...>
    {
      /// \brief This class extends Product so that it can safely perform RAII
      /// reference counting on the shared library.
      public: class ProductWithRefCounter : public Product
      {
        /// \brief Forwarding constructor. Accepts arguments as defined by the
        /// template parameters.
        private: ProductWithRefCounter(Args&&... args)
          : Product(std::forward<Args>(args)...)
        {
          // Do nothing
        }

        /// \brief Reference counter for the plugin instance. This reference
        /// counter ensures that the shared library that provides this plugin
        /// will remain loaded throughout the entire lifetime of the plugin.
        private: std::shared_ptr<void> pluginInstancePtr;

        // Declare friendship
        friend class Producing;
      };

      // Documentation inherited
      public: ReturnType Construct(Args&&... _args) override
      {
        std::unique_ptr<ProductWithRefCounter> product(
              new ProductWithRefCounter(std::forward<Args>(_args)...));

        product->pluginInstancePtr = this->PluginInstancePtrFromThis();
        return product;
      }
    };
  }
}

#endif
