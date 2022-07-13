# Gazebo Plugin

**Maintainer:** grey [AT] openrobotics [DOT] org

[![GitHub open issues](https://img.shields.io/github/issues-raw/gazebosim/gz-plugin.svg)](https://github.com/gazebosim/gz-plugin/issues)
[![GitHub open pull requests](https://img.shields.io/github/issues-pr-raw/gazebosim/gz-plugin.svg)](https://github.com/gazebosim/gz-plugin/pulls)
[![Discourse topics](https://img.shields.io/discourse/https/community.gazebosim.org/topics.svg)](https://community.gazebosim.org)
[![Hex.pm](https://img.shields.io/hexpm/l/plug.svg)](https://www.apache.org/licenses/LICENSE-2.0)

Build | Status
-- | --
Test coverage | [![codecov](https://codecov.io/gh/gazebosim/gz-plugin/branch/master/graph/badge.svg)](https://codecov.io/gh/gazebosim/gz-plugin/branch/master)
Ubuntu Focal | [![Build Status](https://build.osrfoundation.org/job/ignition_plugin-ci-master-focal-amd64/badge/icon)](https://build.osrfoundation.org/job/ignition_plugin-ci-master-focal-amd64/)
Homebrew      | [![Build Status](https://build.osrfoundation.org/buildStatus/icon?job=ignition_plugin-ci-main-homebrew-amd64)](https://build.osrfoundation.org/job/ignition_plugin-ci-main-homebrew-amd64)
Windows 7     | [![Build Status](https://build.osrfoundation.org/job/ignition_plugin-ci-master-windows7-amd64/badge/icon)](https://build.osrfoundation.org/job/ignition_plugin-ci-master-windows7-amd64/)

** Library for registering plugin libraries and dynamically loading them at runtime.**

Gazebo Plugin is a component in the Gazebo framework, a set
of libraries designed to rapidly develop robot applications.

[http://gazebosim.org](http://gazebosim.org)

## Features

* Registration of **templated and non-templated** classes from any translation unit.
* Querying of interfaces **by C++ type or name string**.
* **Dynamically load** plugins at runtime according to the interfaces they provide.
* **Reference counting** of plugin objects to unload libraries no longer in use.
* **Command line tool** `gz plugin` to inspect plugins.

## Installation

See the [installation tutorial](https://gazebosim.org/api/plugin/1.2/installation.html).

# Documentation

API documentation and tutorials can be accessed at [https://gazebosim.org/libs/plugin](https://gazebosim.org/libs/plugin)
