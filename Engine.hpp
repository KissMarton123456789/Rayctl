#pragma once
#include <SDL2/SDL.h>
#include <vector>
#include <cstring>
#include "stb_image.hpp"
#include "Player.hpp"
#include "HitResult.hpp"


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
    void update();
    void drawWall();
    void render3D();
    HitResult performDDA(Vec2 rayDir);
    void drawWallColumn(int x, int drawStart, int drawEnd, uint32_t color, std::vector<uint32_t>&frameBuffer, int screenWidth);
    std::vector<uint32_t> loadTexture(const char* filepath);


    int screenWidth;
    int screenHeight;
    bool isRuning;

    SDL_Window* window;
    SDL_Renderer* renderer;
    SDL_Texture* texture;

    std::vector<uint32_t> framebuffer;
    Player player;
    std::vector<uint32_t> wallTexture;
    std::vector<uint32_t> floorTexture;
    std::vector<uint32_t> ceilingTexture;
};