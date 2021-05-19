# Examples

These examples demonstrate various Ignition Plugin features.

## Build

After installing Ignition Plugin, from source or from binaries, build with:

```
git clone https://github.com/ignitionrobotics/ign-plugin/
cd ign-plugin/examples
mkdir build
cd build
cmake ..
```

### Ubuntu and MacOS

```bash
make
```

### Windows

```bash
cmake --build . --config Release
```

## Run

Two executables are created inside the `build` folder for each example.

You can run each executable from the build folder with `./executablename`. For example:

### Ubuntu and MacOS

`./robot`

### Windows

`.\Release\robot.exe`
