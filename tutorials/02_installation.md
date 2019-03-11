\page installation Installation

This tutorial describes how to install Ignition Plugin on [Ubuntu Linux](#ubuntu_install) and [macOS](#macos_install) via either a binary distribution or from source. Support for Windows is coming soon.


## Ubuntu ## {#ubuntu_install}


Ignition Plugin version 1 requires Ubuntu Bionic, while version 0
can be used with Ubuntu Xenial.

If you don't already have the `lsb-release` package installed, please do so now:
```{.sh}
sudo apt-get update
sudo apt-get install lsb-release
```

Setup your computer to accept software from
[packages.osrfoundation.org](http://packages.osrfoundation.org):

```{.sh}
sudo sh -c 'echo "deb http://packages.osrfoundation.org/gazebo/ubuntu-stable `lsb_release -cs` main" > /etc/apt/sources.list.d/gazebo-stable.list'
```

Setup keys:

```{.sh}
wget http://packages.osrfoundation.org/gazebo.key -O - | sudo apt-key add -
```

Install Ignition Plugin

```{.sh}
sudo apt-get update
sudo apt-get install libignition-plugin-dev
```

**Build from source**

Run the following to install dependencies
```{.sh}
sudo apt-get install libignition-cmake2-dev
```

Clone the ign-plugin repository from bitbucket
```{.sh}
hg clone https://bitbucket.org/ignitionrobotics/ign-plugin
```

Then build using CMake
```{.sh}
cd ign-plugin
mkdir build
cd build
cmake ..
make
sudo make install
```

## macOS ## {#macos_install}

Ignition Plugin and several of its dependencies can be installed on macOS
with [Homebrew](http://brew.sh/) using the [osrf/simulation
tap](https://github.com/osrf/homebrew-simulation). Ignition Plugin version 1
requires macOS High Sierra (10.13) or later, while version 0 supports
Sierra (10.12) or later.

**Install Binaries using Homebrew**

Install homebrew, which should also prompt you to install the XCode
command-line tools:

```{.sh}
ruby -e "$(curl -fsSL https://raw.githubusercontent.com/Homebrew/install/master/install)"
```

Run the following commands:

```{.sh}
brew tap osrf/simulation
brew install ignition-plugin1
```

**Build from source using Homebrew**

Run the following to install dependencies
```{.sh}
brew tap osrf/simulation
brew install ignition-plugin1 --only-dependencies
```

Clone the ign-plugin repository from bitbucket
```{.sh}
hg clone https://bitbucket.org/ignitionrobotics/ign-plugin
```

Then build using CMake
```{.sh}
cd ign-plugin
mkdir build
cd build
cmake ..
make
sudo make install
```
