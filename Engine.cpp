#include "Engine.hpp"

constexpr double MOUSE_SENSITIVITY = 0.002;
constexpr int MAP_WIDTH = 20;
constexpr int MAP_HEIGHT = 20;

const int worldMap[MAP_WIDTH][MAP_HEIGHT] = 
{
    {1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1},
    {1,1,1,1,1,0,0,0,0,0,0,0,1,1,1,1,1,0,0,1},
    {1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,1,1},
    {1,0,0,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1},
    {1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,1,1},
    {1,1,1,1,1,0,0,0,0,0,0,0,0,0,0,0,0,1,1,1},
    {1,1,1,1,1,1,1,0,0,0,0,0,1,1,1,1,1,1,1,1},
    {1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
    {1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
    {1,0,0,0,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1},
    {1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
    {1,1,1,1,1,0,0,0,0,0,0,0,0,0,0,0,0,0,1,1},
    {1,1,1,1,1,1,1,1,1,1,0,0,0,0,0,0,0,0,1,1},
    {1,1,1,1,1,1,1,1,1,1,1,1,1,1,0,0,0,0,1,1},
    {1,1,1,1,1,1,1,1,1,1,0,0,0,0,0,0,0,0,1,1},
    {1,1,1,1,1,0,0,0,0,0,0,0,0,0,0,0,0,0,1,1},
    {1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
    {1,0,0,0,0,0,0,0,0,0,1,1,1,1,1,1,1,0,0,1},
    {1,0,0,0,0,0,1,1,1,1,1,1,1,1,0,0,0,0,0,1},
    {1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1}
};


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

    SDL_SetRelativeMouseMode(SDL_TRUE);

    player.isMovingForward  = false;
    player.isMovingBackward = false;
    player.isMovingLeft     = false;
    player.isMovingRight    = false;
    player.isSprinting      = false;

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
        else if(event.type == SDL_MOUSEMOTION)
        {
            double angle = event.motion.xrel * MOUSE_SENSITIVITY;
            player.turn(angle);
        }
        else if(event.type == SDL_KEYDOWN)
        {
            if(event.key.keysym.mod == SDLK_w 
                && event.key.keysym.mod & KMOD_SHIFT) player.isSprinting      = true;
            else if(event.key.keysym.sym == SDLK_w)   player.isMovingForward  = true;
            else if(event.key.keysym.sym == SDLK_s)   player.isMovingBackward = true;
            else if(event.key.keysym.sym == SDLK_a)   player.isMovingLeft     = true;
            else if(event.key.keysym.sym == SDLK_d)   player.isMovingRight    = true;
        }
        else if(event.type == SDL_KEYUP)
        {
            if(event.key.keysym.sym == SDLK_w)         player.isMovingForward  = false;
            else if(event.key.keysym.mod & KMOD_SHIFT) player.isSprinting      = false;
            else if(event.key.keysym.sym == SDLK_s)    player.isMovingBackward = false;
            else if(event.key.keysym.sym == SDLK_a)    player.isMovingLeft     = false;
            else if(event.key.keysym.sym == SDLK_d)    player.isMovingRight    = false;
        }
    }
}

void Engine::update()
{
    double moveSpeed = 80.0/screenWidth;

    if(player.isMovingForward) {
        double nextX = player.pos.x + player.dir.x * moveSpeed;
        double nextY = player.pos.y + player.dir.y * moveSpeed;
        
        if(worldMap[static_cast<int>(nextX)][static_cast<int>(player.pos.y)] == 0) player.pos.x = nextX;
        if(worldMap[static_cast<int>(player.pos.x)][static_cast<int>(nextY)] == 0) player.pos.y = nextY;
    }
    if(player.isMovingBackward) {
        double nextX = player.pos.x - player.dir.x * moveSpeed;
        double nextY = player.pos.y - player.dir.y * moveSpeed;
        
        if(worldMap[static_cast<int>(nextX)][static_cast<int>(player.pos.y)] == 0) player.pos.x = nextX;
        if(worldMap[static_cast<int>(player.pos.x)][static_cast<int>(nextY)] == 0) player.pos.y = nextY;
    }

    if(player.isMovingLeft) {
        double nextX = player.pos.x - player.plane.x * moveSpeed;
        double nextY = player.pos.y - player.plane.y * moveSpeed;
        
        if(worldMap[static_cast<int>(nextX)][static_cast<int>(player.pos.y)] == 0) player.pos.x = nextX;
        if(worldMap[static_cast<int>(player.pos.x)][static_cast<int>(nextY)] == 0) player.pos.y = nextY;
    }
    if(player.isMovingRight) {
        double nextX = player.pos.x + player.plane.x * moveSpeed;
        double nextY = player.pos.y + player.plane.y * moveSpeed;
        
        if(worldMap[static_cast<int>(nextX)][static_cast<int>(player.pos.y)] == 0) player.pos.x = nextX;
        if(worldMap[static_cast<int>(player.pos.x)][static_cast<int>(nextY)] == 0) player.pos.y = nextY;
    }
    if(player.isSprinting) {
        double nextX = player.pos.x + player.dir.x * (moveSpeed * 3);
        double nextY = player.pos.y + player.dir.y * (moveSpeed * 3);
        
        if(worldMap[static_cast<int>(nextX)][static_cast<int>(player.pos.y)] == 0) player.pos.x = nextX;
        if(worldMap[static_cast<int>(player.pos.x)][static_cast<int>(nextY)] == 0) player.pos.y = nextY;
    }
}

void Engine::render()
{
    //IMPORTANT: This will write the pixels 1 by 1 to the screen
    // for (size_t y = 0; y < screenHeight; y++)
    // {
    //     for (size_t x = 0; x < screenWidth; x++)
    //     {
    //         int index = y * screenWidth + x;

    //         uint8_t r = x % 255;
    //         uint8_t g = y % 255;
    //         uint8_t b = 128;

    //         framebuffer[index] = (0xFF << 24) | (r << 16) | (g << 8) | b;
    //     }
    // }
    int halfScreen = (screenWidth * screenHeight) / 2;
    std::fill(framebuffer.begin(), framebuffer.begin() + halfScreen, 0xFF333333);
    std::fill(framebuffer.begin() + halfScreen, framebuffer.end(), 0xFF777777);

    render3D();

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
        update();
        render();
    }
}

// void Engine::processMovement(double moveSpeed, double rotateSpeed)
// {
//     const uint8_t* state = SDL_GetKeyboardState(nullptr);

//     if(state[SDL_SCANCODE_LEFT])
//     {
//         player.turn(rotateSpeed);
//     }
//     if(state[SDL_SCANCODE_RIGHT])
//     {
//         player.turn(-rotateSpeed);
//     }
// }

void Engine::render3D()
{
    for (int x = 0; x < screenWidth; x++) {
        
        double cameraX = 2 * x / static_cast<double>(screenWidth) - 1.0; 
        
        Vec2 rayDir = {
            player.dir.x + player.plane.x * cameraX,
            player.dir.y + player.plane.y * cameraX
        };

        HitResult hit = performDDA(rayDir);

        int lineHeight = static_cast<int>(screenHeight / hit.prepDistance);
        int drawStart = std::max(0, -lineHeight / 2 + screenHeight / 2);
        int drawEnd = std::min(screenHeight - 1, lineHeight / 2 + screenHeight / 2);

        uint32_t wallColor = (hit.axis == WallAxis::Horizontal) ? 0xFF00FFFF : 0xFF5CE85C;
        drawWallColumn(x, drawStart, drawEnd, wallColor, framebuffer, screenWidth);
    }
}

HitResult Engine::performDDA(Vec2 rayDir)
{
    int mapX = static_cast<int>(player.pos.x);
    int mapY = static_cast<int>(player.pos.y);

    double deltaDistX = std::abs(1.0 / rayDir.x);
    double deltaDistY = std::abs(1.0 / rayDir.y);

    double sideDistX, sideDistY;
    int stepX, stepY;

    if (rayDir.x < 0) {
        stepX = -1;
        sideDistX = (player.pos.x - mapX) * deltaDistX;
    } else {
        stepX = 1;
        sideDistX = (mapX + 1.0 - player.pos.x) * deltaDistX;
    }

    if (rayDir.y < 0) {
        stepY = -1;
        sideDistY = (player.pos.y - mapY) * deltaDistY;
    } else {
        stepY = 1;
        sideDistY = (mapY + 1.0 - player.pos.y) * deltaDistY;
    }

    bool hit = false;
    WallAxis side = WallAxis::Vertical;

    while (!hit) {
        if (sideDistX < sideDistY) {
            sideDistX += deltaDistX;
            mapX += stepX;
            side = WallAxis::Vertical;
        } else {
            sideDistY += deltaDistY;
            mapY += stepY;
            side = WallAxis::Horizontal;
        }

        if (mapX < 0 || mapX >= MAP_WIDTH || mapY < 0 || mapY >= MAP_HEIGHT) {
            hit = true; 
        }
        else if (worldMap[mapX][mapY] > 0) {
            hit = true;
        }
    }

    double perpWallDist = (side == WallAxis::Vertical) ? (sideDistX - deltaDistX) : (sideDistY - deltaDistY);

    return {perpWallDist, side};
}

void Engine::drawWallColumn(int x, int drawStart, int drawEnd, uint32_t color, std::vector<uint32_t>&frameBuffer, int screenWidth)
{
    for(int y = drawStart; y <= drawEnd; y++)
    {
        int index = y * screenWidth +x;
        frameBuffer[index] = color;
    }
}