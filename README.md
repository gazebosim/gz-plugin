# Ignition Plugin

** Library for registering plugin libraries and dynamically loading them at runtime.**

Ignition Plugin is a component in the ignition framework, a set
of libraries designed to rapidly develop robot applications.

[http://ignitionrobotics.org](http://ignitionrobotics.org)

Test coverage:

[![codecov](https://codecov.io/bb/ignitionrobotics/ign-plugin/branch/ign-plugin1/graph/badge.svg)](https://codecov.io/bb/ignitionrobotics/ign-plugin)

## Installation

Standard installation can be performed in UNIX systems using the following
steps:

    mkdir build/
    cd build/
    cmake ..
    sudo make install

## Uninstallation

To uninstall the software installed with the previous steps:

    cd build/
    sudo make uninstall

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
