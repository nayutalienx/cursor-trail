# Windows 11 Cursor Trail Fix

## Problem
The cursor trail application was not working properly on Windows 11. The issue was caused by `glfwGetCursorPos()` returning cursor coordinates relative to the window instead of global screen coordinates, which is required for a system-wide cursor trail overlay.

## Solution
This fix implements platform-specific cursor position tracking:

### Windows (including Windows 11)
- Uses the Windows API `GetCursorPos()` function to get global cursor coordinates
- Falls back to GLFW if the Windows API fails
- Improved window creation settings for better Windows 11 compatibility

### Other Platforms (Linux, macOS)
- Continues to use GLFW's `glfwGetCursorPos()` function
- Maintains backward compatibility

## Key Changes

### 1. Global Cursor Position Tracking (`CursorTrail/Game.cpp`)
```cpp
#ifdef _WIN32
    POINT cursorPos;
    if (GetCursorPos(&cursorPos)) {
        xpos = static_cast<double>(cursorPos.x);
        ypos = static_cast<double>(cursorPos.y);
    } else {
        // Fallback to GLFW if Windows API fails
        glfwGetCursorPos(window, &xpos, &ypos);
    }
#else
    // On non-Windows systems, use GLFW
    glfwGetCursorPos(window, &xpos, &ypos);
#endif
```

### 2. Improved Window Setup (`CursorTrail/CursorTrail.cpp`)
- Changed `GLFW_FOCUS_ON_SHOW` to `false` for better Windows 11 compatibility
- Added proper error checking for window creation
- Enabled vsync to reduce CPU usage

### 3. Better Initialization
- Properly initialize trail parts array and current index
- Added error handling and validation

## Building
The project now supports cross-platform compilation:

**Windows:**
```bash
mkdir build
cd build
cmake ..
make  # or use Visual Studio/MSBuild
```

**Linux/macOS:**
```bash
mkdir build
cd build
cmake ..
make
```

## Testing
The fix has been tested to ensure:
- ✅ Cursor trail works correctly on Windows 11
- ✅ Backward compatibility maintained for other platforms
- ✅ Proper error handling and fallback mechanisms
- ✅ Clean resource management

## Technical Details
The root cause was that GLFW's `glfwGetCursorPos()` returns coordinates relative to the window, but for a fullscreen transparent overlay that needs to track the cursor across the entire desktop, we need absolute screen coordinates. Windows 11's window management may have made this more apparent or changed how window-relative coordinates are calculated.

By using `GetCursorPos()` on Windows, we get the actual global cursor position regardless of window focus or layering, ensuring the cursor trail works correctly across the entire desktop.