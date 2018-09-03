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

#ifndef IGNITION_PLUGIN_FACTORY_HH_
#define IGNITION_PLUGIN_FACTORY_HH_

#include <memory>
#include <tuple>

#include <ignition/plugin/EnablePluginFromThis.hh>

namespace ignition
{
  namespace plugin
  {
    /// \brief The Factory class defines a plugin factory that can be used by
    /// the Loader class to produce std::unique_ptr interface classes.
    ///
    /// To define the inputs and outputs of a factory, set the template
    /// arguments, for example:
    ///
    /// \code
    ///   using MyFactory = Factory<InterfaceClass, InputType1, InputType2>;
    /// \endcode
    ///
    /// defines a factory that can produce a `std::unique_ptr<InterfaceClass>`
    /// given arguments of `InputType1` and `InputType2`.
    ///
    /// To register a factory, use the `IGNITION_ADD_FACTORY` macro, e.g.:
    ///
    /// \code
    /// IGNITION_ADD_FACTORY(MyFactory, ImplementedClass)
    /// \endcode
    ///
    /// where `ImplementedClass` is the name of the class that your plugin
    /// library has used to implement `InterfaceClass`.
    template <typename Interface, typename... Args>
    class Factory : public EnablePluginFromThis
    {
      public: using ReturnType = std::unique_ptr<Interface>;

      /// \brief This function is called by Loader to construct the class that
      /// implements the ReturnT interface.
      /// \param[in] _args
      ///   The arguments as defined by the template parameters.
      /// \return an RAII-managed reference to the interface type as defined by
      /// the template parameters.
      public: virtual std::unique_ptr<Interface> Construct(Args&&... _args) = 0;

      /// \private This nested class is used to implement the plugin factory.
      /// It is not intended for external use.
      public: template <typename Product>
      class Producer;
    };
  }
}

#include <ignition/plugin/detail/Factory.hh>

#endif
