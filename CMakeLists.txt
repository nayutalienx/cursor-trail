cmake_minimum_required(VERSION 3.16)
project(CursorTrail)

set(CMAKE_CXX_STANDARD 17)

include_directories(CursorTrail)
include_directories(CursorTrail/include)
include_directories(CursorTrail/lib)
include_directories(CursorTrail/include/glad)
include_directories(CursorTrail/include/GLFW)
include_directories(CursorTrail/include/glm)
include_directories(CursorTrail/include/glm/detail)
include_directories(CursorTrail/include/glm/ext)
include_directories(CursorTrail/include/glm/gtc)
include_directories(CursorTrail/include/glm/gtx)
include_directories(CursorTrail/include/glm/simd)
include_directories(CursorTrail/include/KHR)
include_directories(CursorTrail/include/stb)

# Platform-specific source files
if(WIN32)
    add_executable(CursorTrail
            CursorTrail/lib/glad.c
            CursorTrail/lib/stb.cpp
            CursorTrail/CursorTrail.cpp
            CursorTrail/Game.cpp
            CursorTrail/ResourceManager.cpp
            CursorTrail/Shader.cpp
            CursorTrail/SpriteRenderer.cpp
            CursorTrail/Texture2D.cpp
            CursorTrail/TrailPart.cpp
            CursorTrail/WindowsOverlay.cpp)
else()
    add_executable(CursorTrail
            CursorTrail/lib/glad.c
            CursorTrail/lib/stb.cpp
            CursorTrail/CursorTrail.cpp
            CursorTrail/Game.cpp
            CursorTrail/ResourceManager.cpp
            CursorTrail/Shader.cpp
            CursorTrail/SpriteRenderer.cpp
            CursorTrail/Texture2D.cpp
            CursorTrail/TrailPart.cpp)
endif()

# Platform-specific OpenGL libraries
if(WIN32)
    # Windows-specific libraries - use 64-bit GLFW and add GDI+ for Windows overlay
    set(OpenGlLibs opengl32 gdiplus user32 gdi32 ole32 ${CMAKE_CURRENT_SOURCE_DIR}/CursorTrail/lib/glfw3.lib)
else()
    find_package(PkgConfig REQUIRED)
    pkg_check_modules(GLFW3 REQUIRED glfw3)
    find_package(OpenGL REQUIRED)
    set(OpenGlLibs ${OPENGL_LIBRARIES} ${GLFW3_LIBRARIES})
    include_directories(${GLFW3_INCLUDE_DIRS})
endif()

target_link_libraries(CursorTrail ${OpenGlLibs})