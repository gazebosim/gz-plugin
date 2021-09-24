\page installation Installation

These instructions are for installing only Ignition Plugin.
If you're interested in using all the Ignition libraries, check out this [Ignition installation](https://ignitionrobotics.org/docs/latest/install).

We recommend following the Binary Installation instructions to get up and running as quickly and painlessly as possible.

The Source Installation instructions should be used if you need the very latest software improvements, you need to modify the code, or you plan to make a contribution.

# Ubuntu

## Prerequisites

Ignition Plugin version 1 requires Ubuntu Bionic, while version 0
can be used with Ubuntu Xenial.

If you don't already have the `lsb-release` package installed, please do so now:
```bash
sudo apt-get update
sudo apt-get install lsb-release
```

Setup your computer to accept software from
[packages.osrfoundation.org](http://packages.osrfoundation.org):
```bash
sudo sh -c 'echo "deb http://packages.osrfoundation.org/gazebo/ubuntu-stable `lsb_release -cs` main" > /etc/apt/sources.list.d/gazebo-stable.list'
```

Setup keys:
```bash
wget http://packages.osrfoundation.org/gazebo.key -O - | sudo apt-key add -
```

## Binary Installation

On Ubuntu systems, `apt-get` can be used to install `ignition-plugin`:
```bash
sudo apt-get update
sudo apt install libignition-plugin-dev
```

## Source Installation

1. Install required Ignition dependencies
  ```bash
  sudo apt-get install libignition-cmake2-dev
  ```

1. Install Ignition Tools if you want to use the `ign plugin` command line tool:
  ```bash
  sudo apt-get install ignition-tools

2. Clone the repository
  ```bash
  git clone https://github.com/ignitionrobotics/ign-plugin
  ```

3. Configure and build
  ```bash
  cd ign-plugin
  mkdir build
  cd build
  cmake ..
  make
  ```

4. Optionally, install
  ```bash
  sudo make install
  ```

# macOS

## Prerequisites

Ignition Plugin and several of its dependencies can be installed on macOS
with [Homebrew](http://brew.sh/) using the [osrf/simulation
tap](https://github.com/osrf/homebrew-simulation). Ignition Plugin version 1
requires macOS High Sierra (10.13) or later, while version 0 supports
Sierra (10.12) or later.

## Binary Installation

1. Install Homebrew, which should also prompt you to install the XCode command-line tools
  ```bash
  ruby -e "$(curl -fsSL https://raw.githubusercontent.com/Homebrew/install/master/install)"
  ```

2. Run the following commands
  ```bash
  brew tap osrf/simulation
  brew install ignition-plugin1
  ```

## Source Installation

1. Install dependencies
  ```bash
  brew tap osrf/simulation
  brew install ignition-plugin1 --only-dependencies
  ```

2. Clone the repository
  ```bash
  git clone https://github.com/ignitionrobotics/ign-plugin
  ```

3. Configure and build
  ```bash
  cd ign-plugin
  mkdir build
  cd build
  cmake ..
  make
  ```

4. Optionally, install
  ```bash
  sudo make install
  ```

# Windows

## Prerequisites

First, follow the [ign-cmake](https://github.com/ignitionrobotics/ign-cmake) tutorial for installing Conda, Visual Studio, CMake, and other prerequisites, and also for creating a Conda environment.

Navigate to `condabin` if necessary to use the `conda` command (i.e., if Conda is not in your `PATH` environment variable. You can find the location of `condabin` in Anaconda Prompt, `where conda`).

Create if necessary, and activate a Conda environment:
```bash
conda create -n ign-ws
conda activate ign-ws
```

## Binary Installation

```bash
conda install libignition-plugin<#> --channel conda-forge
```

Be sure to replace `<#>` with a number value, such as 2 or 3, depending on
which version you need.

## Source Installation

This assumes you have created and activated a Conda environment while installing the Prerequisites.

1. Install Ignition dependencies:

  You can view available versions and their dependencies:
  ```bash
  conda search libignition-plugin* --channel conda-forge --info
  ```

  Install dependencies, replacing `<#>` with the desired version:
  ```bash
  conda install libignition-cmake<#> --channel conda-forge
  ```

2. Navigate to where you would like to build the library, and clone the repository.
  ```bash
  # Optionally, append `-b ign-plugin#` (replace # with a number) to check out a specific version
  git clone https://github.com/ignitionrobotics/ign-plugin
  ```

3. Configure and build
  ```bash
  cd ign-plugin
  mkdir build
  cd build
  cmake .. -DBUILD_TESTING=OFF  # Optionally, -DCMAKE_INSTALL_PREFIX=path\to\install
  cmake --build . --config Release
  ```

4. Optionally, install
  ```bash
  cmake --install . --config Release
  ```

# Documentation

API documentation and tutorials can be accessed at
[Ignition Robotics website](https://ignitionrobotics.org/libs/plugin)

You can also generate the documentation from a clone of this repository by following these steps.

1. You will need [Doxygen](http://www.doxygen.org/). On Ubuntu Doxygen can be installed using
  ```bash
  sudo apt-get install doxygen
  ```

2. Clone the repository
  ```bash
  git clone https://github.com/ignitionrobotics/ign-plugin
  ```

3. Configure and build the documentation.
  ```bash
  cd ign-plugin
  mkdir build
  cd build
  cmake ..
  make doc
  ```

4. View the documentation by running the following command from the `build` directory.
  ```bash
  firefox doxygen/html/index.html
  ```

**Note** Alternatively, documentation for `ignition-plugin` can be found within the source code, and also in the MIGRATION.md guide.


# Test

Run tests as follows:
```bash
make test
```

Tests are automatically built. To disable them, run `cmake` as follows:
```bash
cmake .. -DBUILD_TESTING=false
```

## Test coverage

To run test coverage:

1. Install LCOV

  ```bash
  sudo apt-get install lcov
  ```

2. Build with coverage

  ```bash
  cd build/
  cmake .. -DCMAKE_BUILD_TYPE=coverage
  make
  ```

3. Run tests
  ```bash
  make test
  ```

4. Generate coverage
  ```bash
  make coverage
  ```

5. View results
  ```bash
  firefox coverage/index.html
  ```
