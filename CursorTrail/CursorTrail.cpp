#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include "Game.h"
#include "ResourceManager.h"
#include "Config.h"

#ifdef _WIN32
#include "WindowsOverlay.h"
#endif

#include <iostream>

// GLFW function declarations
void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void key_callback(GLFWwindow* window, int key, int scancode, int action, int mode);

Game gameObject;

int main(int argc, char* argv[])
{
    // Initialize configuration system
    std::cout << "Cursor Trail - Customizable Version" << std::endl;
    
    // Try to load config from default file
    g_config.LoadFromFile("config.ini");
    
    // Parse command line arguments (override config file)
    if (g_config.ParseCommandLine(argc, argv) == false && argc > 1) {
        // If help was shown or there was an error, exit
        return 0;
    }
    
    // Print current configuration
    g_config.PrintConfig();
#ifdef _WIN32
    // Use Windows-specific overlay implementation for guaranteed top-level transparent overlay
    std::cout << "Starting Windows overlay mode for guaranteed transparency and top-level display..." << std::endl;
    
    WindowsOverlay overlay;
    if (!overlay.Initialize()) {
        std::cerr << "Failed to initialize Windows overlay. Falling back to OpenGL mode." << std::endl;
        // Fall through to OpenGL implementation
    } else {
        std::cout << "Windows overlay initialized successfully. Press Ctrl+C to exit." << std::endl;
        
        // Main loop for Windows overlay
        MSG msg = {};
        auto lastUpdate = GetTickCount64();
        
        while (true) {
            // Process Windows messages
            while (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE)) {
                if (msg.message == WM_QUIT) {
                    goto cleanup;
                }
                TranslateMessage(&msg);
                DispatchMessage(&msg);
            }
            
            // Update and render at ~60fps
            auto currentTime = GetTickCount64();
            if (currentTime - lastUpdate >= 16) { // ~60fps
                overlay.Update();
                overlay.Render();
                lastUpdate = currentTime;
            }
            
            // Small sleep to prevent 100% CPU usage
            Sleep(1);
        }
        
    cleanup:
        overlay.Cleanup();
        return 0;
    }
#endif

    // OpenGL implementation (Windows fallback and other platforms)
    std::cout << "Starting OpenGL mode..." << std::endl;
    
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    
    // Window hints for cursor trail overlay
    glfwWindowHint(GLFW_MOUSE_PASSTHROUGH, true);
    glfwWindowHint(GLFW_TRANSPARENT_FRAMEBUFFER, true);
    glfwWindowHint(GLFW_FLOATING, true);

    // Improved Windows 11 compatibility - overlay should not take focus
    glfwWindowHint(GLFW_VISIBLE, true);
    glfwWindowHint(GLFW_FOCUS_ON_SHOW, false);  // Set to false for proper overlay behavior on Windows 11
    glfwWindowHint(GLFW_DECORATED, false);

    const GLFWvidmode* mode =  glfwGetVideoMode(glfwGetPrimaryMonitor());
    gameObject.Width = mode->width;
    gameObject.Height = mode->height;

    #ifdef __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
    #endif
    glfwWindowHint(GLFW_RESIZABLE, false);

    GLFWwindow* window = glfwCreateWindow(gameObject.Width, gameObject.Height, "CursorTrail", nullptr, nullptr);
    if (!window)
    {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }
    
    glfwMakeContextCurrent(window);

    // Enable vsync to reduce CPU usage
    glfwSwapInterval(1);

    //glfwSetWindowOpacity(window, 0.7);

    // glad: load all OpenGL function pointers
    // ---------------------------------------
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cout << "Failed to initialize GLAD" << std::endl;
        glfwTerminate();
        return -1;
    }

    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

    // OpenGL configuration
    // --------------------
    glViewport(0, 0, gameObject.Width, gameObject.Height);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    
    // Set clear color to transparent for proper overlay transparency
    glClearColor(0.0f, 0.0f, 0.0f, 0.0f);

    // initialize game
    // ---------------
    gameObject.Init();

    while (!glfwWindowShouldClose(window))
    {
        glfwPollEvents();

        // update game state
        // -----------------
        gameObject.Update(window);

        // render
        // ------
        glClear(GL_COLOR_BUFFER_BIT);
        gameObject.Render();

        glfwSwapBuffers(window);
    }

    // delete all resources as loaded using the resource manager
    // ---------------------------------------------------------
    ResourceManager::Clear();

    glfwTerminate();
    return 0;
}

void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    // make sure the viewport matches the new window dimensions; note that width and 
    // height will be significantly larger than specified on retina displays.
    glViewport(0, 0, width, height);
}