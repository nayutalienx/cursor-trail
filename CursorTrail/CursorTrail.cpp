#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include "Game.h"
#include "ResourceManager.h"

#include <iostream>

// GLFW function declarations
void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void key_callback(GLFWwindow* window, int key, int scancode, int action, int mode);

Game gameObject;

int main(int argc, char* argv[])
{
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    
    // killer hints
    glfwWindowHint(GLFW_MOUSE_PASSTHROUGH, true);
    glfwWindowHint(GLFW_TRANSPARENT_FRAMEBUFFER, true);
    glfwWindowHint(GLFW_FLOATING, true);

    // Set it always, because in the background render stop working
    glfwWindowHint(GLFW_VISIBLE, true);
    glfwWindowHint(GLFW_FOCUS_ON_SHOW, true);
    glfwWindowHint(GLFW_DECORATED, false);

    

    const GLFWvidmode* mode =  glfwGetVideoMode(glfwGetPrimaryMonitor());
    gameObject.Width = mode->width;
    gameObject.Height = mode->height;



    #ifdef __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
    #endif
    glfwWindowHint(GLFW_RESIZABLE, false);

    GLFWwindow* window = glfwCreateWindow(gameObject.Width, gameObject.Height, "game", nullptr, nullptr);
    glfwMakeContextCurrent(window);


    //glfwSetWindowOpacity(window, 0.7);

    // glad: load all OpenGL function pointers
    // ---------------------------------------
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return -1;
    }

    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

    // OpenGL configuration
    // --------------------
    glViewport(0, 0, gameObject.Width, gameObject.Height);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

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