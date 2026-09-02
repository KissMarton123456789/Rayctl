#include "Engine.hpp"

Engine::Engine(int width, int height)
    : screenWidth(width), screenHeight(height), isRuning(false),
      window(nullptr), renderer(nullptr), texture(nullptr)
{
    framebuffer.resize(screenWidth * screenHeight, 0);
}

Engine::~Engine()
{
    if (texture) SDL_DestroyTexture(texture);
    if (renderer) SDL_DestroyRenderer(renderer);
    if (window) SDL_DestroyWindow(window);
    SDL_Quit();
}

bool Engine::init()
{
    if(SDL_Init(SDL_INIT_VIDEO) < 0) return false;

    window = SDL_CreateWindow("Raycaster", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, screenWidth, screenHeight, SDL_WINDOW_SHOWN);
    if (!window) return false;

    renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
    texture = SDL_CreateTexture(
        renderer,
        SDL_PIXELFORMAT_ABGR8888,
        SDL_TEXTUREACCESS_STREAMING,
        screenWidth,
        screenHeight
    );

    return true;
}

void Engine::processInput()
{
    SDL_Event event;
    while(SDL_PollEvent(&event))
    {
        if (event.type == SDL_QUIT ||
            (event.type == SDL_KEYDOWN && event.key.keysym.sym == SDLK_ESCAPE))
        {
            isRuning = false;
        }
    }
}

void Engine::render()
{
    //IMPORTANT: This will write the pixels 1 by 1 to the screen
    for (size_t y = 0; y < screenHeight; y++)
    {
        for (size_t x = 0; x < screenWidth; x++)
        {
            int index = y * screenWidth + x;

            uint8_t r = x % 255;
            uint8_t g = y % 255;
            uint8_t b = 128;

            framebuffer[index] = (0xFF << 24) | (r << 16) | (g << 8) | b;
        }
    }

    //Push the CPU buffer to the GPU
    SDL_UpdateTexture(texture, nullptr, framebuffer.data(), screenWidth * sizeof(uint32_t));
    
    SDL_RenderClear(renderer);
    SDL_RenderCopy(renderer, texture, nullptr, nullptr);
    SDL_RenderPresent(renderer);
}

void Engine::run()
{
    isRuning = true;
    while (isRuning)
    {
        processInput();
        render();
    }
}