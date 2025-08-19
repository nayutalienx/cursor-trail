# cursor-trail
Interpolated cursor trail written in OpenGL

![alt text](https://github.com/nayutalienx/cursor-trail/blob/master/cursor_trail.gif?raw=true)

## Download Windows Executable

You can download a pre-compiled Windows executable from the [GitHub Actions artifacts](../../actions). Look for the latest successful build and download the `cursor-trail-windows-x64` artifact.

## Building from Source

### Windows
1. Install CMake and Visual Studio 2022
2. Clone the repository
3. Run the build commands:
```bash
mkdir build
cd build
cmake .. -G "Visual Studio 17 2022" -A x64
cmake --build . --config Release
```

### Linux/macOS
1. Install CMake and GLFW development libraries
2. Clone the repository
3. Run the build commands:
```bash
mkdir build
cd build
cmake .. -DCMAKE_BUILD_TYPE=Release
make
```

## Windows 11 Compatibility

This project has been updated to work properly on Windows 11 by implementing global cursor position tracking using the Windows API. The cursor trail now displays correctly across the entire desktop on Windows 11 systems.

