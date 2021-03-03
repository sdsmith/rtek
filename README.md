# RTek Game Engine [![Build status](https://ci.appveyor.com/api/projects/status/vhg77c4q2dan0l16?svg=true)](https://ci.appveyor.com/project/sdsmith/rtek)

Experimental game engine.

## Building the project

### Windows

> Note: The commands that follow assume you are working in Cygwin.

Visual Studio solution:
```
cmake -S. -B./build -G"Visual Studio 16 2019"
```

The solution is present in the `build` folder.

To make a compilation database (`compile_commands.json`) for external tools:
```
cmake -S. -B./build -G"NMake Makefiles" -DCMAKE_EXPORT_COMPILE_COMMANDS=ON
```

Only the configure step is required to make the compilation database. For ease of integration, make a symlink to it in the root of the project:
```
ln -s ./build/compile_commands.json compile_commands.json
```
