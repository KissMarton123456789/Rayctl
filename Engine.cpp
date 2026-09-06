#define STB_IMAGE_IMPLEMENTATION
#include "Engine.hpp"
#include <cstring>

constexpr double MOUSE_SENSITIVITY = 0.002;
constexpr int MAP_WIDTH = 100;
constexpr int MAP_HEIGHT = 100;
constexpr int TEX_WIDTH = 64;
constexpr int TEX_HEIGHT = 64;

std::vector<int> worldMap(MAP_WIDTH * MAP_HEIGHT, 0);
inline int getTile(int x, int y) {
    return worldMap[x * MAP_HEIGHT + y];
}

inline void setTile(int x, int y, int value) {
    worldMap[x * MAP_HEIGHT + y] = value;
}

Engine::Engine(int width, int height)
    : screenWidth(width), screenHeight(height), isRuning(false),
      window(nullptr), renderer(nullptr), texture(nullptr)
{
    framebuffer.resize(screenWidth * screenHeight, 0);
    worldMap = generateMap();
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

    
    int width, height, channels;
    unsigned char* wallImage = stbi_load("doom_texture_2.png", &width, &height, &channels, 4);
    unsigned char* floorImage = stbi_load("doom_floor_texture_2.png", &width, &height, &channels, 4);
    unsigned char* ceilingImage = stbi_load("doom_ceiling_texture_1.png", &width, &height, &channels, 4);
    if (!wallImage || !floorImage || !ceilingImage) {
        return false; 
    }

    wallTexture.resize(width * height);
    floorTexture.resize(width * height);
    ceilingTexture.resize(width * height);
    
    std::memcpy(wallTexture.data(), wallImage, width * height * sizeof(uint32_t));
    std::memcpy(floorTexture.data(), floorImage, width * height * sizeof(uint32_t));
    std::memcpy(ceilingTexture.data(), ceilingImage, width * height * sizeof(uint32_t));
    
    stbi_image_free(wallImage);
    stbi_image_free(floorImage);
    stbi_image_free(ceilingImage);

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
            if(event.key.keysym.sym == SDLK_w)      player.isMovingForward  = true;
            else if(event.key.keysym.sym == SDLK_s) player.isMovingBackward = true;
            else if(event.key.keysym.sym == SDLK_a) player.isMovingLeft     = true;
            else if(event.key.keysym.sym == SDLK_d) player.isMovingRight    = true;

            if(event.key.keysym.sym == SDLK_LSHIFT || event.key.keysym.sym == SDLK_RSHIFT) {
                player.isSprinting = true;
            }
        }
        else if(event.type == SDL_KEYUP)
        {
            if(event.key.keysym.sym == SDLK_w)      player.isMovingForward  = false;
            else if(event.key.keysym.sym == SDLK_s) player.isMovingBackward = false;
            else if(event.key.keysym.sym == SDLK_a) player.isMovingLeft     = false;
            else if(event.key.keysym.sym == SDLK_d) player.isMovingRight    = false;

            if(event.key.keysym.sym == SDLK_LSHIFT || event.key.keysym.sym == SDLK_RSHIFT) {
                player.isSprinting = false;
            }
        }
    }
}

void Engine::update()
{
    double moveSpeed = 80.0/screenWidth;

    if(player.isMovingForward) 
    {
        double nextX = player.pos.x + player.dir.x * moveSpeed;
        double nextY = player.pos.y + player.dir.y * moveSpeed;
        
        if(getTile(static_cast<int>(nextX), static_cast<int>(player.pos.y)) == 0) player.pos.x = nextX;
        if(getTile(static_cast<int>(player.pos.x), static_cast<int>(nextY)) == 0) player.pos.y = nextY;
    }
    if(player.isMovingBackward) 
    {
        double nextX = player.pos.x - player.dir.x * moveSpeed;
        double nextY = player.pos.y - player.dir.y * moveSpeed;
        
        if(getTile(static_cast<int>(nextX), static_cast<int>(player.pos.y)) == 0) player.pos.x = nextX;
        if(getTile(static_cast<int>(player.pos.x), static_cast<int>(nextY)) == 0) player.pos.y = nextY;
    }

    if(player.isMovingLeft) 
    {
        double nextX = player.pos.x - player.plane.x * moveSpeed;
        double nextY = player.pos.y - player.plane.y * moveSpeed;
        
        if(getTile(static_cast<int>(nextX), static_cast<int>(player.pos.y)) == 0) player.pos.x = nextX;
        if(getTile(static_cast<int>(player.pos.x), static_cast<int>(nextY)) == 0) player.pos.y = nextY;
    }
    if(player.isMovingRight) 
    {
        double nextX = player.pos.x + player.plane.x * moveSpeed;
        double nextY = player.pos.y + player.plane.y * moveSpeed;
        
        if(getTile(static_cast<int>(nextX), static_cast<int>(player.pos.y)) == 0) player.pos.x = nextX;
        if(getTile(static_cast<int>(player.pos.x), static_cast<int>(nextY)) == 0) player.pos.y = nextY;
    }
    if(player.isSprinting) 
    {
        double nextX = player.pos.x + player.dir.x * (moveSpeed * 3);
        double nextY = player.pos.y + player.dir.y * (moveSpeed * 3);
        
        if(getTile(static_cast<int>(nextX), static_cast<int>(player.pos.y)) == 0) player.pos.x = nextX;
        if(getTile(static_cast<int>(player.pos.x), static_cast<int>(nextY)) == 0) player.pos.y = nextY;
    }
}

void Engine::render()
{
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


void Engine::render3D()
{
    //Floor
    for (int y = screenHeight / 2 + 1; y < screenHeight; ++y) {
        Vec2 rayDirLeft = { player.dir.x - player.plane.x, player.dir.y - player.plane.y };
        Vec2 rayDirRight = { player.dir.x + player.plane.x, player.dir.y + player.plane.y };

        double p = y - screenHeight / 2.0;
        double rowDistance = (0.5 * screenHeight) / p;

        Vec2 floorStep = (rayDirRight - rayDirLeft) * (rowDistance / screenWidth);
        Vec2 floorPos = player.pos + rayDirLeft * rowDistance;

        for (int x = 0; x < screenWidth; ++x) {
            int texX = static_cast<int>(TEX_WIDTH * (floorPos.x - std::floor(floorPos.x))) & (TEX_WIDTH - 1);
            int texY = static_cast<int>(TEX_HEIGHT * (floorPos.y - std::floor(floorPos.y))) & (TEX_HEIGHT - 1);

            // Render Floor
            framebuffer[y * screenWidth + x] = floorTexture[TEX_HEIGHT * texY + texX];
            framebuffer[(screenHeight - y - 1) * screenWidth + x] = ceilingTexture[TEX_HEIGHT * texY + texX];

            floorPos += floorStep;
        }
    }

    // Walls
    for (int x = 0; x < screenWidth; x++) 
    {
        
        double cameraX = 2 * x / static_cast<double>(screenWidth) - 1.0; 
        
        Vec2 rayDir = {
            player.dir.x + player.plane.x * cameraX,
            player.dir.y + player.plane.y * cameraX
        };
        
        HitResult hit = performDDA(rayDir);
        
        int lineHeight = static_cast<int>(screenHeight / hit.prepDistance);
        int drawStart = std::max(0, -lineHeight / 2 + screenHeight / 2);
        int drawEnd = std::min(screenHeight - 1, lineHeight / 2 + screenHeight / 2);
        
        double wallX; 
        if (hit.axis == WallAxis::Vertical) {
            wallX = player.pos.y + hit.prepDistance * rayDir.y;
        } else {
            wallX = player.pos.x + hit.prepDistance * rayDir.x;
        }
        wallX -= std::floor(wallX);

        int texX = static_cast<int>(wallX * static_cast<double>(TEX_WIDTH));

        // Flip texture coordinate to prevent mirroring
        if ((hit.axis == WallAxis::Vertical && rayDir.x > 0) || 
            (hit.axis == WallAxis::Horizontal && rayDir.y < 0)) {
            texX = TEX_WIDTH - texX - 1;
        }
        
        double step = 1.0 * TEX_HEIGHT / lineHeight;
        double texPos = (drawStart - screenHeight / 2.0 + lineHeight / 2.0) * step;

        for (int y = drawStart; y <= drawEnd; ++y) {
            int texY = static_cast<int>(texPos) & (TEX_HEIGHT - 1);
            texPos += step;

            uint32_t color = wallTexture[TEX_HEIGHT * texY + texX];

            if (hit.axis == WallAxis::Horizontal) {
                color = (color >> 1) & 0x7F7F7F7F; 
            }

            framebuffer[y * screenWidth + x] = color;
        }
    }
}

//THIS METHOD IS AI GENERATED
std::vector<int> Engine::generateMap() {
    // 1. Fill entire map grid with solid walls (1)
    std::vector<int> map(MAP_WIDTH * MAP_HEIGHT, 1);

    std::random_device rd;
    std::mt19937 gen(rd());

    std::vector<Room> rooms;

    // Guaranteed starting room around player spawn point (3.5, 3.5)
    rooms.push_back({ 2, 2, 6, 6 });

    // Generate random room count based on map dimensions
    int targetRooms = (MAP_WIDTH * MAP_HEIGHT) / 120;
    std::uniform_int_distribution<int> sizeDist(5, 10);

    for (int i = 0; i < targetRooms; ++i) {
        int w = sizeDist(gen);
        int h = sizeDist(gen);
        
        // Keep rooms at least 2 tiles away from absolute map edges
        int x = std::uniform_int_distribution<int>(2, MAP_WIDTH - w - 3)(gen);
        int y = std::uniform_int_distribution<int>(2, MAP_HEIGHT - h - 3)(gen);

        rooms.push_back({ x, y, w, h });
    }

    // 2. Carve out rooms (set tiles to 0)
    for (const auto& r : rooms) {
        for (int rx = r.x; rx < r.x + r.w; ++rx) {
            for (int ry = r.y; ry < r.y + r.h; ++ry) {
                map[rx * MAP_HEIGHT + ry] = 0;
            }
        }
    }

    // 3. Connect rooms sequentially with L-shaped corridors
    for (size_t i = 1; i < rooms.size(); ++i) {
        int prevX = rooms[i - 1].centerX();
        int prevY = rooms[i - 1].centerY();
        int currX = rooms[i].centerX();
        int currY = rooms[i].centerY();

        // Randomize turn direction (Horizontal-first vs Vertical-first)
        if (std::uniform_int_distribution<int>(0, 1)(gen) == 0) {
            for (int x = std::min(prevX, currX); x <= std::max(prevX, currX); ++x) {
                map[x * MAP_HEIGHT + prevY] = 0;
            }
            for (int y = std::min(prevY, currY); y <= std::max(prevY, currY); ++y) {
                map[currX * MAP_HEIGHT + y] = 0;
            }
        } else {
            for (int y = std::min(prevY, currY); y <= std::max(prevY, currY); ++y) {
                map[prevX * MAP_HEIGHT + y] = 0;
            }
            for (int x = std::min(prevX, currX); x <= std::max(prevX, currX); ++x) {
                map[x * MAP_HEIGHT + currY] = 0;
            }
        }
    }

    // 4. Force solid perimeter border to prevent ray casting out-of-bounds
    for (int x = 0; x < MAP_WIDTH; ++x) {
        map[x * MAP_HEIGHT + 0] = 1;
        map[x * MAP_HEIGHT + (MAP_HEIGHT - 1)] = 1;
    }
    for (int y = 0; y < MAP_HEIGHT; ++y) {
        map[0 * MAP_HEIGHT + y] = 1;
        map[(MAP_WIDTH - 1) * MAP_HEIGHT + y] = 1;
    }

    return map;
}

HitResult Engine::performDDA(Vec2 rayDir)
{
    int mapX = static_cast<int>(player.pos.x);
    int mapY = static_cast<int>(player.pos.y);
    
    double deltaDistX = std::abs(1.0 / rayDir.x);
    double deltaDistY = std::abs(1.0 / rayDir.y);
    
    double sideDistX, sideDistY;
    int stepX, stepY;
    
    if (rayDir.x < 0) 
    {
        stepX = -1;
        sideDistX = (player.pos.x - mapX) * deltaDistX;
    } 
    else 
    {
        stepX = 1;
        sideDistX = (mapX + 1.0 - player.pos.x) * deltaDistX;
    }
    
    if (rayDir.y < 0) 
    {
        stepY = -1;
        sideDistY = (player.pos.y - mapY) * deltaDistY;
    } 
    else 
    {
        stepY = 1;
        sideDistY = (mapY + 1.0 - player.pos.y) * deltaDistY;
    }
    
    bool hit = false;
    WallAxis side = WallAxis::Vertical;
    
    while (!hit) 
    {
        if (sideDistX < sideDistY) 
        {
            sideDistX += deltaDistX;
            mapX += stepX;
            side = WallAxis::Vertical;
        } 
        else 
        {
            sideDistY += deltaDistY;
            mapY += stepY;
            side = WallAxis::Horizontal;
        }
        
        if (mapX < 0 || mapX >= MAP_WIDTH || mapY < 0 || mapY >= MAP_HEIGHT) 
        {
            hit = true; 
        }
        else if (getTile(mapX, mapY) > 0) 
        {
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

std::vector<uint32_t> Engine::loadTexture(const char* filepath) {
    int w, h, channels;
    unsigned char* img = stbi_load(filepath, &w, &h, &channels, 4);
    
    std::vector<uint32_t> buffer(TEX_WIDTH * TEX_HEIGHT);
    uint32_t* rawPixels = reinterpret_cast<uint32_t*>(img);
    
    for (int i = 0; i < TEX_WIDTH * TEX_HEIGHT; ++i) 
    {
        buffer[i] = rawPixels[i];
    }
    
    stbi_image_free(img);
    return buffer;
}