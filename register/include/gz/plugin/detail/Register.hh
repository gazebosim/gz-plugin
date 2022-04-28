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


#ifndef IGNITION_PLUGIN_DETAIL_REGISTER_HH_
#define IGNITION_PLUGIN_DETAIL_REGISTER_HH_

#include <set>
#include <string>
#include <typeinfo>
#include <type_traits>
#include <utility>

#include <ignition/utils/SuppressWarning.hh>

#include <ignition/plugin/EnablePluginFromThis.hh>
#include <ignition/plugin/Info.hh>
#include <ignition/plugin/utility.hh>


#if defined _WIN32 || defined __CYGWIN__
  #ifdef __GNUC__
    #define DETAIL_IGN_PLUGIN_VISIBLE __attribute__ ((dllexport))
  #else
    #define DETAIL_IGN_PLUGIN_VISIBLE __declspec(dllexport)
  #endif
#else
  #if __GNUC__ >= 4
    #define DETAIL_IGN_PLUGIN_VISIBLE __attribute__ ((visibility ("default")))
  #else
    #define DETAIL_IGN_PLUGIN_VISIBLE
  #endif
#endif

// extern "C" ensures that the symbol name of IgnitionPluginHook
// does not get mangled by the compiler, so we can easily use dlsym(~) to
// retrieve it.
extern "C"
{
  /// \private IgnitionPluginHook is the hook that's used by the Loader to
  /// retrieve Info from a shared library that provides plugins.
  ///
  /// The symbol is explicitly exported (visibility is turned on) using
  /// DETAIL_IGN_PLUGIN_VISIBLE to ensure that dlsym(~) is able to find it.
  ///
  /// DO NOT CALL THIS FUNCTION DIRECTLY OR CREATE YOUR OWN IMPLEMENTATION OF IT
  /// This function is used by the Registrar and Loader classes. Nothing else
  /// should be using it.
  ///
  /// \param[in] _inputSingleInfo
  ///   This argument is used by Registrar to input a single instance of
  ///   plugin::Info data. Loader will set this to a nullptr when trying to
  ///   receive data from the hook.
  ///
  /// \param[out] _outputAllInfo
  ///   Loader will pass in a pointer to a pointer of an InfoMap pertaining to
  ///   the highest API version that it knows of. If this IgnitionPluginHook was
  ///   built against a version of ign-plugin that provides an equal or greater
  ///   API version, then IgnitionPluginHook will modify *_outputAllInfo to
  ///   point at its internal &InfoMap that corresponds to the requested API
  ///   version, which is identified by _inputAndOutputAPIVersion.
  ///
  ///   If _inputAndOutputAPIVersion is greater than the highest API version
  ///   known by this IgnitionPluginHook, then IgnitionPluginHook will not
  ///   modify _outputAllInfo, and instead it will change the value pointed to
  ///   by _inputAndOutputAPIVersion so that it points to the highest API
  ///   version known by this IgnitionPluginHook. At that point, Loader can call
  ///   this function again, but using the older API version which known by this
  ///   IgnitionPluginHook.
  ///
  /// \param[in,out] _inputAndOutputAPIVersion
  ///   Loader will pass in a pointer to the highest API version that it knows.
  ///   If that API version is higher than what this IgnitionPluginHook is
  ///   compatible with, then this IgnitionPluginHook will change the value
  ///   pointed to by _inputAndOutputAPIVersion to the value of the highest API
  ///   version that it knows.
  ///
  /// \param[in,out] _inputAndOutputInfoSize
  ///   This input/output parameter is used for sanity checking. The Loader
  ///   inputs a pointer to the size that it expects for the Info data
  ///   structure, and IgnitionPluginHook verifies that this expectation matches
  ///   its own Info size. Then, IgnitionPluginHook will overwrite the value
  ///   pointed to so that it matches its own Info size value.
  ///
  /// \param[in,out] _inputAndOutputInfoAlign
  ///   Similar to _inputAndOutputInfoSize, this is used for sanity checking. It
  ///   inspects and returns the alignof(Info) value instead of the sizeof(Info)
  ///   value.
  DETAIL_IGN_PLUGIN_VISIBLE void IgnitionPluginHook(
      const void *_inputSingleInfo,
      const void ** const _outputAllInfo,
      int *_inputAndOutputAPIVersion,
      std::size_t *_inputAndOutputInfoSize,
      std::size_t *_inputAndOutputInfoAlign)
#ifdef IGN_PLUGIN_REGISTER_MORE_TRANS_UNITS
  ; /* NOLINT */
#else
  // ATTENTION: If you get a linking error complaining about
  // multiple definitions of IgnitionPluginHook,
  // then make sure that all but one of your
  // library's translation units (.cpp files) includes the
  // <ignition/plugin/RegisterMore.hh> header instead of
  // <ignition/plugin/Register.hh>.
  //
  // Only ONE and exactly ONE .cpp file in your library should include
  // Register.hh. All the rest should include RegisterMore.hh. It does not
  // matter which .cpp file you choose, as long as it gets compiled into your
  // plugin library.
  // ^^^^^^^^^^^^^^^^^^^^^ READ ABOVE FOR LINKING ERRORS ^^^^^^^^^^^^^^^^^^^^^
  {
    using InfoMap = ignition::plugin::InfoMap;
    // We use a static variable here so that we can accumulate multiple
    // Info objects from multiple plugin registration calls within one
    // shared library, and then provide it all to the Loader through this
    // single hook.
    static InfoMap pluginMap;

    if (_inputSingleInfo)
    {
      // When _inputSingleInfo is not a nullptr, it means that one of the plugin
      // registration macros is providing us with some Info.
      const ignition::plugin::Info *input =
          static_cast<const ignition::plugin::Info*>(_inputSingleInfo);

      InfoMap::iterator it;
      bool inserted;

      // We use insert(~) to ensure that we do not accidentally overwrite some
      // existing information for the plugin that has this name.
      std::tie(it, inserted) =
          pluginMap.insert(std::make_pair(input->name, *input));

      if (!inserted)
      {
        // If the object was not inserted, then an entry already existed for
        // this plugin type. We should still insert each of the interface map
        // entries and aliases provided by the input info, just in case any of
        // them are missing from the currently existing entry. This allows the
        // user to specify different interfaces and aliases for the same plugin
        // type using different macros in different locations or across multiple
        // translation units.
        ignition::plugin::Info &entry = it->second;

        for (const auto &interfaceMapEntry : input->interfaces)
          entry.interfaces.insert(interfaceMapEntry);

        for (const auto &aliasSetEntry : input->aliases)
          entry.aliases.insert(aliasSetEntry);
      }
    }

    if (_outputAllInfo)
    {
      // When _outputAllInfo is not a nullptr, it means that a Loader is
      // trying to retrieve Info from us.

      // The Loader should provide valid pointers to these fields as part
      // of a handshake procedure.
      if (nullptr == _inputAndOutputAPIVersion ||
          nullptr == _inputAndOutputInfoSize ||
          nullptr == _inputAndOutputInfoAlign)
      {
        // This should never happen, or else the function is being misused.
        // LCOV_EXCL_START
        return;
        // LCOV_EXCL_STOP
      }

      bool agreement = true;

      if (ignition::plugin::INFO_API_VERSION != *_inputAndOutputAPIVersion)
      {
        // LCOV_EXCL_START
        agreement = false;
        // LCOV_EXCL_STOP
      }

      if (sizeof(ignition::plugin::Info) != *_inputAndOutputInfoSize)
      {
        // LCOV_EXCL_START
        agreement = false;
        // LCOV_EXCL_STOP
      }

      if (alignof(ignition::plugin::Info) != *_inputAndOutputInfoAlign)
      {
        // LCOV_EXCL_START
        agreement = false;
        // LCOV_EXCL_STOP
      }

      // The handshake parameters that were passed into us are overwritten with
      // the values that we have on our end. That way, if our Info API is
      // lower than that of the Loader, then the Loader will know
      // to call this function using an older version of Info, and then
      // convert it to the newer version on the loader side.
      //
      // This implementation might change when new API versions are introduced,
      // but this current implementation will still be forward compatible with
      // new API versions.
      *_inputAndOutputAPIVersion = ignition::plugin::INFO_API_VERSION;
      *_inputAndOutputInfoSize = sizeof(ignition::plugin::Info);
      *_inputAndOutputInfoAlign = alignof(ignition::plugin::Info);

      // If the size, alignment, or API do not agree, we should return without
      // outputting any of the plugin info; otherwise, we could get a
      // segmentation fault.
      //
      // We will return the current API version to the Loader, and it may
      // then decide to attempt the call to this function again with the correct
      // API version if it supports backwards/forwards compatibility.
      if (!agreement)
      {
        // LCOV_EXCL_START
        return;
        // LCOV_EXCL_STOP
      }

      *_outputAllInfo = &pluginMap;
    }
  }
#endif
}

namespace ignition
{
  namespace plugin
  {
    namespace detail
    {
      //////////////////////////////////////////////////
      /// \brief This default will be called when NoMoreInterfaces is an empty
      /// parameter pack. When one or more Interfaces are provided, the other
      /// template specialization of this class will be called.
      template <typename PluginClass, typename... NoMoreInterfaces>
      struct InterfaceHelper
      {
        public: static void InsertInterfaces(Info::InterfaceCastingMap &)
        {
          // Do nothing. This is the terminal specialization of the variadic
          // template class member function.
        }
      };

      //////////////////////////////////////////////////
      /// \brief This specialization will be called when one or more Interfaces
      /// are specified.
      template <typename PluginClass, typename Interface,
                typename... RemainingInterfaces>
      struct InterfaceHelper<PluginClass, Interface, RemainingInterfaces...>
      {
        public: static void InsertInterfaces(
          Info::InterfaceCastingMap &interfaces)
        {
          // READ ME: If you get a compilation error here, then one of the
          // interfaces that you tried to register for your plugin is not
          // actually a base class of the plugin class. This is not allowed. A
          // plugin class must inherit every interface class that you want it to
          // provide.
          static_assert(std::is_base_of<Interface, PluginClass>::value,
                        "YOU ARE ATTEMPTING TO REGISTER AN INTERFACE FOR A "
                        "PLUGIN, BUT THE INTERFACE IS NOT A BASE CLASS OF THE "
                        "PLUGIN.");

          interfaces.insert(std::make_pair(
                typeid(Interface).name(),
                [=](void* v_ptr)
                {
                    PluginClass *d_ptr = static_cast<PluginClass*>(v_ptr);
                    return static_cast<Interface*>(d_ptr);
                }));

          InterfaceHelper<PluginClass, RemainingInterfaces...>
              ::InsertInterfaces(interfaces);
        }
      };

      //////////////////////////////////////////////////
      /// \brief This overload will be called when no more aliases remain to be
      /// inserted. If one or more aliases still need to be inserted, then the
      /// overload below this one will be called instead.
      inline void InsertAlias(std::set<std::string> &/*aliases*/)
      {
        // Do nothing. This is the terminal overload of the variadic template
        // function.
      }

      template <typename... Aliases>
      void InsertAlias(std::set<std::string> &aliases,
                       const std::string &nextAlias,
                       Aliases&&... remainingAliases)
      {
        aliases.insert(nextAlias);
        InsertAlias(aliases, std::forward<Aliases>(remainingAliases)...);
      }

      //////////////////////////////////////////////////
      template <typename PluginClass, bool DoEnablePluginFromThis>
      struct IfEnablePluginFromThisImpl
      {
        public: static void AddIt(Info::InterfaceCastingMap &_interfaces)
        {
          _interfaces.insert(std::make_pair(
                  typeid(EnablePluginFromThis).name(),
                  [=](void *v_ptr)
                  {
                    PluginClass *d_ptr = static_cast<PluginClass*>(v_ptr);
                    return static_cast<EnablePluginFromThis*>(d_ptr);
                  }));
        }
      };

      //////////////////////////////////////////////////
      template <typename PluginClass>
      struct IfEnablePluginFromThisImpl<PluginClass, false>
      {
        public: static void AddIt(Info::InterfaceCastingMap &)
        {
          // Do nothing, because the plugin does not inherit
          // the EnablePluginFromThis interface.
        }
      };

      //////////////////////////////////////////////////
      template <typename PluginClass>
      struct IfEnablePluginFromThis
          : IfEnablePluginFromThisImpl<PluginClass,
                std::is_base_of<EnablePluginFromThis, PluginClass>::value>
      { }; // NOLINT

      //////////////////////////////////////////////////
      /// \brief This specialization of the Register class will be called when
      /// one or more arguments are provided to the IGNITION_ADD_PLUGIN(~)
      /// macro. This is the only version of the Registrar class that is allowed
      /// to compile.
      template <typename PluginClass, typename... Interfaces>
      struct Registrar
      {
        public: static Info MakeInfo()
        {
          Info info;

          // Set the name of the plugin
          info.name = typeid(PluginClass).name();

          // Create a factory for generating new plugin instances
          info.factory = [=]()
          {
            // vvvvvvvvvvvvvvvvvvvvvvvv  READ ME  vvvvvvvvvvvvvvvvvvvvvvvvvvvvv
            // If you get a compilation error here, then you are trying to
            // register an abstract class as a plugin, which is not allowed. To
            // register a plugin class, every one if its virtual functions must
            // have a definition.
            //
            // Read through the error produced by your compiler to see which
            // pure virtual functions you are neglecting to provide overrides
            // for.
            // ^^^^^^^^^^^^^ READ ABOVE FOR COMPILATION ERRORS ^^^^^^^^^^^^^^^^
            return static_cast<void*>(new PluginClass);
          };

IGN_UTILS_WARN_IGNORE__NON_VIRTUAL_DESTRUCTOR
          // Create a deleter to clean up destroyed instances
          info.deleter = [=](void *ptr)
          {
            delete static_cast<PluginClass*>(ptr);
          };
IGN_UTILS_WARN_RESUME__NON_VIRTUAL_DESTRUCTOR

          // Construct a map from the plugin to its interfaces
          InterfaceHelper<PluginClass, Interfaces...>
              ::InsertInterfaces(info.interfaces);

          return info;
        }

        /// \brief This function registers a plugin along with a set of
        /// interfaces that it provides.
        public: static void Register()
        {
          // Make all info that the user has specified
          Info info = MakeInfo();

          // Add the EnablePluginFromThis interface automatically if it is
          // inherited by PluginClass.
          IfEnablePluginFromThis<PluginClass>::AddIt(info.interfaces);

          // Send this information as input to this library's global repository
          // of plugins.
          IgnitionPluginHook(&info, nullptr, nullptr, nullptr, nullptr);
        }


        public: template <typename... Aliases>
        static void RegisterAlias(Aliases&&... aliases)
        {
          // Dev note (MXG): We expect the RegisterAlias function to be called
          // using the IGNITION_ADD_PLUGIN_ALIAS(~) macro, which should never
          // contain any interfaces. Therefore, this parameter pack should be
          // empty.
          //
          // In the future, we could allow Interfaces and Aliases to be
          // specified simultaneously, but that would be very tricky to do with
          // macros, so for now we will enforce this assumption to make sure
          // that the implementation is working as expected.
          static_assert(sizeof...(Interfaces) == 0,
                        "THERE IS A BUG IN THE ALIAS REGISTRATION "
                        "IMPLEMENTATION! PLEASE REPORT THIS!");

          Info info = MakeInfo();

          // Gather up all the aliases that have been specified for this plugin.
          InsertAlias(info.aliases, std::forward<Aliases>(aliases)...);

          // Send this information as input to this library's global repository
          // of plugins.
          IgnitionPluginHook(&info, nullptr, nullptr, nullptr, nullptr);
        }
      };
    }
  }
}

//////////////////////////////////////////////////
/// This macro creates a uniquely-named class whose constructor calls the
/// ignition::plugin::detail::Registrar::Register function. It then declares a
/// uniquely-named instance of the class with static lifetime. Since the class
/// instance has a static lifetime, it will be constructed when the shared
/// library is loaded. When it is constructed, the Register function will
/// be called.
#define DETAIL_IGNITION_ADD_PLUGIN_HELPER(UniqueID, ...) \
  namespace ignition \
  { \
    namespace plugin \
    { \
      namespace \
      { \
        struct ExecuteWhenLoadingLibrary##UniqueID \
        { \
          ExecuteWhenLoadingLibrary##UniqueID() \
          { \
            ::ignition::plugin::detail::Registrar<__VA_ARGS__>::Register(); \
          } \
        }; \
  \
        static ExecuteWhenLoadingLibrary##UniqueID execute##UniqueID; \
      } /* namespace */ \
    } \
  }


//////////////////////////////////////////////////
/// This macro is needed to force the __COUNTER__ macro to expand to a value
/// before being passed to the *_HELPER macro.
#define DETAIL_IGNITION_ADD_PLUGIN_WITH_COUNTER(UniqueID, ...) \
  DETAIL_IGNITION_ADD_PLUGIN_HELPER(UniqueID, __VA_ARGS__)


//////////////////////////////////////////////////
/// We use the __COUNTER__ here to give each plugin registration its own unique
/// name, which is required in order to statically initialize each one.
#define DETAIL_IGNITION_ADD_PLUGIN(...) \
  DETAIL_IGNITION_ADD_PLUGIN_WITH_COUNTER(__COUNTER__, __VA_ARGS__)


//////////////////////////////////////////////////
/// This macro creates a uniquely-named class whose constructor calls the
/// ignition::plugin::detail::Registrar::RegisterAlias function. It then
/// declares a uniquely-named instance of the class with static lifetime. Since
/// the class instance has a static lifetime, it will be constructed when the
/// shared library is loaded. When it is constructed, the Register function will
/// be called.
#define DETAIL_IGNITION_ADD_PLUGIN_ALIAS_HELPER(UniqueID, PluginClass, ...) \
  namespace ignition \
  { \
    namespace plugin \
    { \
      namespace \
      { \
        struct ExecuteWhenLoadingLibrary##UniqueID \
        { \
          ExecuteWhenLoadingLibrary##UniqueID() \
          { \
            ::ignition::plugin::detail::Registrar<PluginClass>::RegisterAlias( \
                __VA_ARGS__); \
          } \
        }; \
  \
        static ExecuteWhenLoadingLibrary##UniqueID execute##UniqueID; \
      } /* namespace */ \
    } \
  }


//////////////////////////////////////////////////
/// This macro is needed to force the __COUNTER__ macro to expand to a value
/// before being passed to the *_HELPER macro.
#define DETAIL_IGNITION_ADD_PLUGIN_ALIAS_WITH_COUNTER( \
  UniqueID, PluginClass, ...) \
  DETAIL_IGNITION_ADD_PLUGIN_ALIAS_HELPER(UniqueID, PluginClass, __VA_ARGS__)


//////////////////////////////////////////////////
/// We use the __COUNTER__ here to give each plugin registration its own unique
/// name, which is required in order to statically initialize each one.
#define DETAIL_IGNITION_ADD_PLUGIN_ALIAS(PluginClass, ...) \
  DETAIL_IGNITION_ADD_PLUGIN_ALIAS_WITH_COUNTER( \
  __COUNTER__, PluginClass, __VA_ARGS__)


//////////////////////////////////////////////////
#define DETAIL_IGNITION_ADD_FACTORY(ProductType, FactoryType) \
  DETAIL_IGNITION_ADD_PLUGIN(FactoryType::Producing<ProductType>, FactoryType) \
  DETAIL_IGNITION_ADD_PLUGIN_ALIAS( \
      FactoryType::Producing<ProductType>, \
      ::ignition::plugin::DemangleSymbol(typeid(ProductType).name()))


//////////////////////////////////////////////////
#define DETAIL_IGNITION_ADD_FACTORY_ALIAS(ProductType, FactoryType, ...) \
  DETAIL_IGNITION_ADD_FACTORY(ProductType, FactoryType) \
  DETAIL_IGNITION_ADD_PLUGIN_ALIAS(FactoryType::Producing<ProductType>, \
      __VA_ARGS__)

#endif
