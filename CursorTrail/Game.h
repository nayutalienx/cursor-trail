#ifndef GAME_H
#define GAME_H

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include "TrailPart.h"

// Represents the current state of the game
enum GameState {
    GAME_ACTIVE,
    GAME_MENU,
    GAME_WIN
};

const int MAX_SPRITES = 2048;

// Game holds all game-related state and functionality.
// Combines all game-related data into a single class for
// easy access to each of the components and manageability.
class Game
{
public:
    // game state
    GameState               State;
    TrailPart               parts[MAX_SPRITES];
    int                     currentIndex;
    unsigned int            Width, Height;
    // constructor/destructor
    Game();
    ~Game();
    // initialize game state (load all shaders/textures/levels)
    void Init();
    // game loop
    void Update(GLFWwindow* window);
    void Render();
    void AddPart(TrailPart part);
};

#endif