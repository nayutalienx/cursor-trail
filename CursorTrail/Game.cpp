#include "Game.h"
#include "SpriteRenderer.h"
#include "ResourceManager.h"

#ifdef _WIN32
#include <windows.h>
#endif


Game::Game() : State(GAME_ACTIVE), currentIndex(0)
{
    // Initialize trail parts array to ensure clean state
    for (int i = 0; i < MAX_SPRITES; i++) {
        parts[i] = TrailPart(0.0f, 0.0f, 0.0f);
    }
}

Game::~Game()
{

}

SpriteRenderer* Renderer;

void Game::Init()
{
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
    ResourceManager::LoadTexture("cursortrail.png", true, "trail");
}

const float SPRITE_SIZE = 15;
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

    TrailPart currentTrail = TrailPart(xpos, ypos, fadeTime);

    // Add the current cursor position to trail
    this->AddPart(currentTrail);

    // interpolate trail

    int prevIndex;
    if (this->currentIndex != 0) {
        prevIndex = this->currentIndex - 1;
    }
    else {
        prevIndex = MAX_SPRITES - 1;
    }

    TrailPart previousTrail = this->parts[prevIndex];

    glm::vec2 pos1 = glm::vec2(previousTrail.x, previousTrail.y);
    glm::vec2 pos2 = glm::vec2(currentTrail.x, currentTrail.y);

    glm::vec2 diff = pos2 - pos1;
    float distance = glm::length(diff);
    
    // Avoid division by zero
    if (distance > 0.0f) {
        glm::vec2 direction = diff / distance;

        float interval = SPRITE_SIZE / 2.5;
        float stopAt = distance;

        for (float d = interval; d < stopAt; d += interval) {
            glm::vec2 ivec = pos1 + (direction * d);
            this->AddPart(TrailPart(ivec.x, ivec.y, fadeTime));
        }
    }
}

void Game::AddPart(TrailPart part) {

    this->parts[this->currentIndex] = part;

    this->currentIndex++;
    if (this->currentIndex == MAX_SPRITES) {
        this->currentIndex = 0;
    }

}

void Game::Render()
{

    Texture2D tex;
    tex = ResourceManager::GetTexture("trail");

    for (int i = 0; i < MAX_SPRITES; i++) {

        float newTime = this->parts[i].time - 0.05;
        if (newTime < 0) {
            newTime = 0;
        }

        this->parts[i].time = newTime;

        TrailPart part = this->parts[i];


        float alpha = part.time;

        Renderer->DrawSprite(
            tex,
            glm::vec2(part.x-(SPRITE_SIZE/2.0), part.y-(SPRITE_SIZE/2.0)),
            glm::vec2(SPRITE_SIZE, SPRITE_SIZE),
            0,
            alpha);
    }

}