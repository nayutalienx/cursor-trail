#include "Game.h"
#include "SpriteRenderer.h"
#include "ResourceManager.h"
#include <iostream>

#ifdef _WIN32
#include <windows.h>
#endif


Game::Game() : State(GAME_ACTIVE), currentIndex(0), parts(nullptr)
{
}

Game::~Game()
{
    delete[] parts;
}

SpriteRenderer* Renderer;

void Game::Init()
{
    // Initialize dynamic trail parts array based on configuration
    parts = new TrailPart[g_config.maxParticles];
    for (int i = 0; i < g_config.maxParticles; i++) {
        parts[i] = TrailPart(0.0f, 0.0f, 0.0f);
    }
    
    // load shaders
    ResourceManager::LoadShader("sprite.vs", "sprite.frag", nullptr, "sprite");
    // configure shaders
    glm::mat4 projection = glm::ortho(0.0f, static_cast<float>(this->Width),
        static_cast<float>(this->Height), 0.0f, -1.0f, 1.0f);
    ResourceManager::GetShader("sprite").Use().SetInteger("image", 0);
    ResourceManager::GetShader("sprite").SetMatrix4("projection", projection);
    // set render-specific controls

    Shader shader;
    shader = ResourceManager::GetShader("sprite");

    Renderer = new SpriteRenderer(shader);
    // Load texture from config
    ResourceManager::LoadTexture(g_config.texturePath.c_str(), true, "trail");
    
    std::cout << "Game initialized with " << g_config.maxParticles << " max particles, texture: " << g_config.texturePath << std::endl;
}

const float fadeTime = 1.0;

void Game::Update(GLFWwindow* window)
{
    double xpos, ypos;
    
    // Get global cursor position for proper system-wide cursor trail
    // This fixes the issue on Windows 11 where glfwGetCursorPos returns
    // window-relative coordinates instead of screen coordinates
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
    // On non-Windows systems, use GLFW (may need adjustment for Linux/macOS)
    glfwGetCursorPos(window, &xpos, &ypos);
#endif

    TrailPart currentTrail = TrailPart(xpos, ypos, g_config.fadeTime);

    // Add the current cursor position to trail
    this->AddPart(currentTrail);

    // interpolate trail

    int prevIndex;
    if (this->currentIndex != 0) {
        prevIndex = this->currentIndex - 1;
    }
    else {
        prevIndex = g_config.maxParticles - 1;
    }

    TrailPart previousTrail = this->parts[prevIndex];

    glm::vec2 pos1 = glm::vec2(previousTrail.x, previousTrail.y);
    glm::vec2 pos2 = glm::vec2(currentTrail.x, currentTrail.y);

    glm::vec2 diff = pos2 - pos1;
    float distance = glm::length(diff);
    
    // Avoid division by zero
    if (distance > 0.0f) {
        glm::vec2 direction = diff / distance;

        float interval = g_config.spawnFrequency;
        float stopAt = distance;

        for (float d = interval; d < stopAt; d += interval) {
            glm::vec2 ivec = pos1 + (direction * d);
            this->AddPart(TrailPart(ivec.x, ivec.y, g_config.fadeTime));
        }
    }
}

void Game::AddPart(TrailPart part) {

    this->parts[this->currentIndex] = part;

    this->currentIndex++;
    if (this->currentIndex == g_config.maxParticles) {
        this->currentIndex = 0;
    }

}

void Game::Render()
{

    Texture2D tex;
    tex = ResourceManager::GetTexture("trail");

    for (int i = 0; i < g_config.maxParticles; i++) {

        float newTime = this->parts[i].time - g_config.fadeRate;
        if (newTime < 0) {
            newTime = 0;
        }

        this->parts[i].time = newTime;

        TrailPart part = this->parts[i];


        float alpha = part.time;

        Renderer->DrawSprite(
            tex,
            glm::vec2(part.x-(g_config.spriteSize/2.0), part.y-(g_config.spriteSize/2.0)),
            glm::vec2(g_config.spriteSize, g_config.spriteSize),
            0,
            alpha);
    }

}