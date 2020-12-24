\page installation Installation

This tutorial describes how to install Ignition Plugin via either a binary distribution or from source.

## Ubuntu

### Prerequisites

Ignition Plugin versions 1+ require Ubuntu Bionic, while version 0
can be used with Ubuntu Xenial.

If you don't already have the `lsb-release` package installed, please do so now:
```
sudo apt-get update
sudo apt-get install lsb-release
```

Setup your computer to accept software from
[packages.osrfoundation.org](http://packages.osrfoundation.org):

```
sudo sh -c 'echo "deb http://packages.osrfoundation.org/gazebo/ubuntu-stable `lsb_release -cs` main" > /etc/apt/sources.list.d/gazebo-stable.list'
```

Setup keys:

```
wget http://packages.osrfoundation.org/gazebo.key -O - | sudo apt-key add -
```

### Binary Install

```
sudo apt-get update
sudo apt-get install libignition-plugin-dev
```

### Source Install

Run the following to install dependencies
```
sudo apt-get install libignition-cmake2-dev
```

Clone the ign-plugin repository from GitHub
```
git clone https://github.com/ignitionrobotics/ign-plugin
```

Then build using CMake
```
cd ign-plugin
mkdir build
cd build
cmake ..
make
sudo make install
```

## macOS

### Prerequisites

Ignition Plugin and several of its dependencies can be installed on macOS
with [Homebrew](http://brew.sh/) using the [osrf/simulation
tap](https://github.com/osrf/homebrew-simulation). Ignition Plugin versions 1+
require macOS High Sierra (10.13) or later, while version 0 supports
Sierra (10.12) or later.

### Install Binaries using Homebrew

Install homebrew, which should also prompt you to install the XCode
command-line tools:

```
ruby -e "$(curl -fsSL https://raw.githubusercontent.com/Homebrew/install/master/install)"
```

Run the following commands:

```
brew tap osrf/simulation
brew install ignition-plugin2
```

### Build from source using Homebrew

Run the following to install dependencies
```
brew tap osrf/simulation
brew install ignition-plugin2 --only-dependencies
```

Clone the ign-plugin repository from GitHub
```
git clone https://github.com/ignitionrobotics/ign-plugin
```

Then build using CMake
```
cd ign-plugin
mkdir build
cd build
cmake ..
make
sudo make install
```

## Windows

### Prerequisites

First, follow the [ign-cmake](https://github.com/ignitionrobotics/ign-cmake) tutorial for installing Conda, Visual Studio, CMake, etc., prerequisites, and creating a Conda environment.

Navigate to ``condabin`` if necessary to use the ``conda`` command (i.e., if Conda is not in your `PATH` environment variable. You can find the location of ``condabin`` in Anaconda Prompt, ``where conda``).

Create if necessary, and activate a Conda environment:

```
conda create -n ign-ws
conda activate ign-ws
```

Install Ignition dependencies:

You can view the list of dependencies, replacing `<#>` with the desired version:

```
conda search libignition-plugin<#> --channel conda-forge --info
```

Install dependencies, replacing `<#>` with the desired versions:

```
conda install libignition-cmake<#> --channel conda-forge
```

### Binary Install

```
conda install libignition-plugin<#> --channel conda-forge
```

Be sure to replace `<#>` with a number value, such as 1 or 2, depending on
which version you need.

### Source Install

1. Navigate to where you would like to build the library, and clone the repository.

    ```
    # Optionally, append `-b ign-plugin#` (replace # with a number) to check out a specific version
    git clone https://github.com/ignitionrobotics/ign-plugin.git
    ```

1. Configure and build

    ```
    cd ign-plugin
    mkdir build
    cd build
    cmake .. -DBUILD_TESTING=OFF  # Optionally, -DCMAKE_INSTALL_PREFIX=path\to\install
    cmake --build . --config Release
    ```

1. Optionally, install

    ```
    cmake --install . --config Release
    ```
