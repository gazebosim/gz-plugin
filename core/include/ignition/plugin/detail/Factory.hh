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


#include <iostream>




namespace ignition
{
  namespace plugin
  {
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
      /// \brief The class ProductWithRefCounter needs to call the Product
      /// destructor before the parentPluginInstancePtr member gets destructed,
      /// because the destructor of Product depends on the library that this
      /// Product's plugin was loaded from. Therefore, we create a base class to
      /// hold this reference counter, and we list this class first in the
      /// base-specifier-list of ProductWithRefCounter so that it gets
      /// constructed first and destructed last.
      public: class RefCounter
      {
        /// \brief Reference counter for the plugin instance. This reference
        /// counter ensures that the shared library that provides this plugin
        /// will remain loaded throughout the entire lifetime of the plugin.
//        private: std::shared_ptr<void> parentPluginInstancePtr;
        public: PluginPtr parentPluginInstancePtr;

        /// \brief Virtual destructor
        public: virtual ~RefCounter()
        {
          std::cout << " ----- Destructing RefCounter ------ " << std::endl;
          std::cout
              << "Internal factory: "
              << this->parentPluginInstancePtr->template QueryInterfaceSharedPtr<Factory<Interface, Args...>>()
              << std::endl;
        }

        // Declare friendship
        friend class Producing;
      };

      /// \brief This class extends Product so that it can safely perform RAII
      /// reference counting on the shared library.
      ///
      /// CRITICAL DEV NOTE(MXG): RefCounter MUST come BEFORE Product in this
      /// base-specifier-list in order to ensure that the library dependency of
      /// Product remains loaded until Product is destructed.
      public: class ProductWithRefCounter : public RefCounter, public Product
      {
        /// \brief Forwarding constructor. Accepts arguments as defined by the
        /// template parameters.
        private: ProductWithRefCounter(Args&&... args)
          : RefCounter(),
            Product(std::forward<Args>(args)...)
        {
          // Do nothing
        }

        // Declare friendship
        friend class Producing;
      };

      // Documentation inherited
      public: InterfacePtr Construct(Args&&... _args) override
      {
        std::unique_ptr<ProductWithRefCounter> product(
              new ProductWithRefCounter(std::forward<Args>(_args)...));

//        product->parentPluginInstancePtr = this->PluginInstancePtrFromThis();
        product->parentPluginInstancePtr = this->PluginFromThis();

        return product;
      }
    };
  }
}

#endif
