/*
 * Copyright (C) 2022 Open Source Robotics Foundation
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

#include <gz/plugin/Register.hh>
#include <ignition/plugin/config.hh>

#define IGNITION_ADD_PLUGIN(PluginClass, ...) \
  GZ_ADD_PLUGIN(PluginClass, __VA_ARGS__)

#define IGNITION_ADD_PLUGIN_ALIAS(PluginClass, ...) \
  GZ_ADD_PLUGIN_ALIAS(PluginClass, __VA_ARGS__)

#define IGNITION_ADD_FACTORY(ProductType, FactoryType) \
  GZ_ADD_FACTORY(ProductType, FactoryType)

#define IGNITION_ADD_FACTORY_ALIAS(ProductType, FactoryType, ...) \
  GZ_ADD_FACTORY_ALIAS(ProductType, FactoryType, __VA_ARGS__)
