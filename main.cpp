#include <iostream>
#include <vector>
#include <algorithm>
#include <SDL3/SDL.h>
#include "glm/glm.hpp"
#include "rectangle.h"
#include "circle.h"

constexpr int WINDOW_WIDTH{700};
constexpr int WINDOW_HEIGHT{800};

constexpr float SHIP_SIZE{50.0f};
constexpr float PROJ_RADIUS{10.0f};
constexpr float PLAYER_PROJ_SPEED{500.0f};                     
constexpr float PLAYER_FIRE_INTERVAL{0.3f};                    
constexpr float ENEMY_PROJ_SPEED{PLAYER_PROJ_SPEED / 5.0f};    
constexpr float ENEMY_FIRE_INTERVAL{PLAYER_FIRE_INTERVAL * 10.0f};
constexpr float ENEMY_FIRE_DELAY{1.0f};                       
constexpr int ENEMY_COUNT{3};

const SDL_Color COLOR_GREEN{0, 200, 0, 255};
const SDL_Color COLOR_ORANGE{255, 140, 0, 255};
const SDL_Color COLOR_RED{255, 0, 0, 255};
const SDL_Color COLOR_MAGENTA{255, 0, 255, 255};
const SDL_Color COLOR_BLUE{0, 0, 255, 255};

SDL_Window *window{nullptr};
SDL_Renderer *renderer{nullptr};
SDL_Event currentEvent;
SDL_Color backgroundColor{255, 255, 255, 255};
bool quit{false};
float displayScale{1.0f};
float dt{0.0f};
struct Projectile {
    Circle shape;     
    glm::vec2 vel;    
};

enum class GameState { Playing, Won, Lost };

Rectangle player;
std::vector<Rectangle> enemies;
std::vector<Projectile> playerShots;
std::vector<Projectile> enemyShots;
GameState state{GameState::Playing};

float gameTime{0.0f};         
float playerFireCooldown{0.0f};
float enemyFireTimer{0.0f};
bool firing{false};

float winW{WINDOW_WIDTH}, winH{WINDOW_HEIGHT};

void updateWindowSize() {
    int w{0}, h{0};
    SDL_GetCurrentRenderOutputSize(renderer, &w, &h);
    winW = static_cast<float>(w) / displayScale;
    winH = static_cast<float>(h) / displayScale;
}

void clampPlayer() {
    const float half = SHIP_SIZE / 2.0f;
    player.pos.x = std::clamp(player.pos.x, half, std::max(half, winW - half));
    player.pos.y = std::clamp(player.pos.y, half, std::max(half, winH - half));
}

bool circleRectCollision(const Circle &c, const Rectangle &r) {
    const glm::vec2 halfSize(r.width / 2.0f, r.height / 2.0f);
    const glm::vec2 closest = glm::clamp(c.pos, r.pos - halfSize, r.pos + halfSize);
    const glm::vec2 d = c.pos - closest;
    return glm::dot(d, d) <= c.radius * c.radius;
}

bool rectRectCollision(const Rectangle &a, const Rectangle &b) {
    return std::abs(a.pos.x - b.pos.x) < (a.width + b.width) / 2.0f &&
           std::abs(a.pos.y - b.pos.y) < (a.height + b.height) / 2.0f;
}

bool isOffScreen(const Projectile &p) {
    const float r = p.shape.radius;
    return p.shape.pos.x < -r || p.shape.pos.x > winW + r ||
           p.shape.pos.y < -r || p.shape.pos.y > winH + r;
}

void resetGame() {
    updateWindowSize();

     player = Rectangle(SHIP_SIZE, SHIP_SIZE,
                       glm::vec2(winW / 2.0f, winH - SHIP_SIZE / 2.0f),
                       COLOR_GREEN);

    enemies.clear();
    const float sliceW = winW / ENEMY_COUNT;
    for (int i = 0; i < ENEMY_COUNT; i++) {
        const float x = i * sliceW + SHIP_SIZE / 2.0f + SDL_randf() * std::max(0.0f, sliceW - SHIP_SIZE);
        const float y = SHIP_SIZE / 2.0f + SDL_randf() * std::max(0.0f, winH / 2.0f - SHIP_SIZE);
        enemies.emplace_back(SHIP_SIZE, SHIP_SIZE, glm::vec2(x, y), COLOR_ORANGE);
    }

    playerShots.clear();
    enemyShots.clear();
    gameTime = 0.0f;
    playerFireCooldown = 0.0f;
    enemyFireTimer = 0.0f;
    firing = (SDL_GetMouseState(nullptr, nullptr) & SDL_BUTTON_LMASK) != 0;
    state = GameState::Playing;
}

void endGame(GameState result) {
    state = result;
    enemies.clear();
    playerShots.clear();
    enemyShots.clear();
    firing = false;
    player.color = (result == GameState::Won) ? COLOR_BLUE : COLOR_RED;
}

bool initWindow() {
    if (!SDL_Init(SDL_INIT_VIDEO)) {
        SDL_Log("SDL initialization failed: %s\n", SDL_GetError());
        return false;
    }

    displayScale = SDL_GetDisplayContentScale(SDL_GetPrimaryDisplay());
    if (displayScale <= 0.0f) displayScale = 1.0f;

    if (!SDL_CreateWindowAndRenderer("CodeRun7 - Vertical Shooter",
                                     static_cast<int>(displayScale * WINDOW_WIDTH),
                                     static_cast<int>(displayScale * WINDOW_HEIGHT),
                                     SDL_WINDOW_RESIZABLE,
                                     &window, &renderer)) {
        SDL_Log("Failed to create window and renderer: %s\n", SDL_GetError());
        return false;
    }

    SDL_SetRenderScale(renderer, displayScale, displayScale);
    SDL_SetRenderVSync(renderer, 1);

    SDL_SetWindowRelativeMouseMode(window, true);
    return true;
}

void processEvents() {
    while (SDL_PollEvent(&currentEvent)) {
        switch (currentEvent.type) {
            case SDL_EVENT_QUIT:
                quit = true;
                break;

            case SDL_EVENT_WINDOW_RESIZED:
                updateWindowSize();
                clampPlayer();
                break;

            case SDL_EVENT_MOUSE_MOTION:
                
                if (state == GameState::Playing) {
                    player.pos += glm::vec2(currentEvent.motion.xrel, currentEvent.motion.yrel) / displayScale;
                    clampPlayer();
                }
                break;

            case SDL_EVENT_MOUSE_BUTTON_DOWN:
                if (currentEvent.button.button == SDL_BUTTON_LEFT) firing = true;
                break;

            case SDL_EVENT_MOUSE_BUTTON_UP:
                if (currentEvent.button.button == SDL_BUTTON_LEFT) firing = false;
                break;

            case SDL_EVENT_KEY_DOWN:
                if (currentEvent.key.key == SDLK_R && state != GameState::Playing) {
                    resetGame(); 
                } else if (currentEvent.key.key == SDLK_ESCAPE) {
                    quit = true;
                }
                break;

            default:
                break;
        }
    }
}

void updatePlayerShots() {
    playerFireCooldown = std::max(0.0f, playerFireCooldown - dt);
    if (firing && playerFireCooldown <= 0.0f) {
        playerShots.push_back({Circle(PROJ_RADIUS, player.pos, COLOR_RED),
                               glm::vec2(0.0f, -PLAYER_PROJ_SPEED)});
        playerFireCooldown = PLAYER_FIRE_INTERVAL;
    }

    for (auto &p : playerShots) p.shape.pos += p.vel * dt;
    std::erase_if(playerShots, isOffScreen);
}

void updateEnemyShots() {
    if (gameTime >= ENEMY_FIRE_DELAY) {
        enemyFireTimer -= dt;
        if (enemyFireTimer <= 0.0f) {
            for (const auto &e : enemies) {
                glm::vec2 dir = player.pos - e.pos;
                if (glm::dot(dir, dir) > 0.0f) dir = glm::normalize(dir);
                enemyShots.push_back({Circle(PROJ_RADIUS, e.pos, COLOR_MAGENTA), dir * ENEMY_PROJ_SPEED});
            }
            enemyFireTimer += ENEMY_FIRE_INTERVAL;
        }
    }

    for (auto &p : enemyShots) p.shape.pos += p.vel * dt;
    std::erase_if(enemyShots, isOffScreen);
}

void handleCollisions() {
    for (size_t s = 0; s < playerShots.size();) {
        bool hit = false;
        for (size_t e = 0; e < enemies.size(); e++) {
            if (circleRectCollision(playerShots[s].shape, enemies[e])) {
                enemies.erase(enemies.begin() + e);
                hit = true;
                break;
            }
        }
        if (hit) playerShots.erase(playerShots.begin() + s);
        else s++;
    }

    for (const auto &e : enemies) {
        if (rectRectCollision(player, e)) { endGame(GameState::Lost); return; }
    }
    for (const auto &p : enemyShots) {
        if (circleRectCollision(p.shape, player)) { endGame(GameState::Lost); return; }
    }

   if (enemies.empty()) endGame(GameState::Won);
}

void update() {
    if (state != GameState::Playing) return; 
    gameTime += dt;
    updatePlayerShots();
    updateEnemyShots();
    handleCollisions();
}

void drawFrame() {
    SDL_SetRenderDrawColor(renderer, backgroundColor.r, backgroundColor.g, backgroundColor.b, backgroundColor.a);
    SDL_RenderClear(renderer);

   
    for (const auto &e : enemies) FillBorderedRectangle(renderer, e);
    for (auto &p : playerShots) FillBorderedCircle(renderer, p.shape);
    for (auto &p : enemyShots) FillBorderedCircle(renderer, p.shape);
    FillBorderedRectangle(renderer, player);

    SDL_RenderPresent(renderer);
}

void cleanup() {
    if (renderer) { SDL_DestroyRenderer(renderer); renderer = nullptr; }
    if (window) { SDL_DestroyWindow(window); window = nullptr; }
    SDL_Quit();
}

int main(int argc, char *argv[]) {
    if (!initWindow()) {
        std::cout << "Failed to initialize" << std::endl;
        return -1;
    }

    SDL_zero(currentEvent);
    resetGame();

    Uint64 lastTime = SDL_GetTicksNS();
    while (!quit) {
        const Uint64 now = SDL_GetTicksNS();
        dt = static_cast<float>(now - lastTime) / 1e9f;
        lastTime = now;
        dt = std::min(dt, 0.05f); 

        processEvents();
        update();
        drawFrame();
    }

    cleanup();
    return 0;
}