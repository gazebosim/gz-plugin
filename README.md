# Ignition Plugin

**Maintainer:** grey [AT] openrobotics [DOT] org

[![GitHub open issues](https://img.shields.io/github/issues-raw/ignitionrobotics/ign-plugin.svg)](https://github.com/ignitionrobotics/ign-plugin/issues)
[![GitHub open pull requests](https://img.shields.io/github/issues-pr-raw/ignitionrobotics/ign-plugin.svg)](https://github.com/ignitionrobotics/ign-plugin/pulls)
[![Discourse topics](https://img.shields.io/discourse/https/community.gazebosim.org/topics.svg)](https://community.gazebosim.org)
[![Hex.pm](https://img.shields.io/hexpm/l/plug.svg)](https://www.apache.org/licenses/LICENSE-2.0)

Build | Status
-- | --
Test coverage | [![codecov](https://codecov.io/gh/ignitionrobotics/ign-plugin/branch/master/graph/badge.svg)](https://codecov.io/gh/ignitionrobotics/ign-plugin/branch/master)
Ubuntu Bionic | [![Build Status](https://build.osrfoundation.org/job/ignition_plugin-ci-master-bionic-amd64/badge/icon)](https://build.osrfoundation.org/job/ignition_plugin-ci-master-bionic-amd64/)
Homebrew      | [![Build Status](https://build.osrfoundation.org/job/ignition_plugin-ci-master-bionic-amd64/badge/icon)](https://build.osrfoundation.org/job/ignition_plugin-ci-master-bionic-amd64/)
Windows 7     | [![Build Status](https://build.osrfoundation.org/job/ignition_plugin-ci-master-windows7-amd64/badge/icon)](https://build.osrfoundation.org/job/ignition_plugin-ci-master-windows7-amd64/)

** Library for registering plugin libraries and dynamically loading them at runtime.**

Ignition Plugin is a component in the ignition framework, a set
of libraries designed to rapidly develop robot applications.

[http://ignitionrobotics.org](http://ignitionrobotics.org)

## Installation

See the [installation tutorial](https://ignitionrobotics.org/api/plugin/1.1/installation.html).

## Test

Run tests as follows:

    make test

> Tests are automatically built. To disable them, run `cmake` as follows:

      cmake .. -DBUILD_TESTING=false

### Test coverage

To run test coverage:

1. Install LCOV

        sudo apt-get install lcov

1. Build with coverage

        cd build/
        cmake .. -DCMAKE_BUILD_TYPE=coverage
        make

1. Run tests

        make test

1. Generate coverage

        make coverage

1. View results

        firefox coverage/index.html
