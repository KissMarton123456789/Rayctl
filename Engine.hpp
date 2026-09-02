#pragma once
#include <SDL2/SDL.h>
#include <vector>
#include <cstdint>

class Engine
{
public:
    Engine(int width, int height);
    ~Engine();

    bool init();
    void run();
private:
    void render();
    void processInput();

    int screenWidth;
    int screenHeight;
    bool isRuning;

    SDL_Window* window;
    SDL_Renderer* renderer;
    SDL_Texture* texture;

    std::vector<uint32_t> framebuffer;
};