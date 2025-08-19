# Cursor Trail

A beautiful, customizable cursor trail effect that works across your entire desktop. Features dual rendering engines optimized for different platforms.

![Cursor Trail Demo](cursor_trail.gif)

## 🌟 Features

- **Cross-platform compatibility** (Windows, Linux, macOS)
- **Dual rendering engines**:
  - Windows: Native Win32 API with guaranteed top-level transparent overlay
  - Linux/macOS: OpenGL-based rendering
- **True transparency** with no black squares or artifacts
- **Smooth cursor tracking** with interpolated trail effects
- **Customizable trail appearance** (size, fade time, opacity)
- **Low CPU usage** with optimized rendering
- **Click-through support** - doesn't interfere with other applications

## 🎯 Windows 11 Support

This implementation specifically addresses Windows 11 transparency and overlay issues:

- ✅ **Guaranteed top-level display** - Always visible above all applications
- ✅ **True transparency** - No black background artifacts
- ✅ **Proper overlay behavior** - Works as a background overlay, not a full-screen app
- ✅ **Native Windows integration** - Uses Win32 layered windows for optimal performance

## 📥 Download Windows Executable

You can download a pre-compiled Windows executable from the [GitHub Actions artifacts](../../actions). Look for the latest successful build and download the `cursor-trail-windows-x64` artifact.

## 🔧 Building from Source

### Windows (Native Overlay)

1. Install CMake and Visual Studio 2022
2. Clone the repository
3. Run the build commands:
```bash
mkdir build
cd build
cmake .. -G "Visual Studio 17 2022" -A x64
cmake --build . --config Release
```

### Linux/macOS (OpenGL)

1. Install CMake and GLFW development libraries
2. Clone the repository  
3. Run the build commands:
```bash
mkdir build
cd build
cmake .. -DCMAKE_BUILD_TYPE=Release
make
```

## 🚀 Usage

- **Windows**: Double-click `CursorTrail.exe` - automatically uses native overlay
- **Linux/macOS**: Run `./CursorTrail` from terminal - uses OpenGL rendering

Move your cursor to see the trail effect. To exit, close the application window or press `Ctrl+C`.

## ⚙️ Windows 11 Compatibility

This project features a complete rewrite for Windows 11 compatibility:

- **Native Win32 overlay** instead of problematic OpenGL transparency
- **Guaranteed top-level positioning** - no more hidden trails
- **True transparency** - eliminates black square artifacts
- **Global cursor tracking** using Windows API for system-wide trail

See [WINDOWS11_FIX.md](WINDOWS11_FIX.md) for detailed technical information.

## 🎨 Technical Implementation

| Platform | Technology | Transparency | Overlay Support |
|----------|------------|--------------|----------------|
| Windows | Win32 + GDI+ | Perfect | Guaranteed |
| Linux | OpenGL + GLFW | Good | Limited |
| macOS | OpenGL + GLFW | Good | Limited |

The Windows implementation uses layered windows with true alpha blending for the most reliable cursor trail experience on modern Windows systems.

