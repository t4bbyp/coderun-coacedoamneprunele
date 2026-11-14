#ifndef CIRCLE_H
#define CIRCLE_H

#include <SDL3/SDL.h>
#include "glm/glm.hpp"

struct Circle {
    float radius;
    glm::vec2 pos;
    SDL_Color color;

    Circle();
    Circle(float r, glm::vec2 c, SDL_Color color);

    void draw(SDL_Renderer* renderer);
};

inline constexpr SDL_Color CIRCLE_BORDER_BLACK{0, 0, 0, 255};

void plotPoints(SDL_Renderer *renderer, int x, int y, Circle circle);
void plotLines(SDL_Renderer *renderer, int x, int y, Circle circle);
void DrawCircle(SDL_Renderer *renderer, Circle circle);
void FillCircle(SDL_Renderer *renderer, Circle circle);
void FillBorderedCircle(SDL_Renderer *renderer, Circle circle,
                        SDL_Color borderColor = CIRCLE_BORDER_BLACK,
                        int thickness = 1);

#endif //CIRCLE_H