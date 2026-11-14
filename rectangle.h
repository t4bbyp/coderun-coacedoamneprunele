#ifndef RECTANGLE_H
#define RECTANGLE_H

#include <SDL3/SDL.h>
#include "glm/glm.hpp"

struct Rectangle {
    glm::vec2 pos;
    glm::vec2 vel;
    glm::vec2 acc;
    float width;
    float height;
    SDL_Color color;

    Rectangle();
    Rectangle(float width, float height, glm::vec2 pos, SDL_Color color);
    void draw(SDL_Renderer* renderer);
};

inline constexpr SDL_Color BORDER_BLACK{0, 0, 0, 255};

void DrawRectangle(SDL_Renderer *renderer, Rectangle rectangle);
void FillRectangle(SDL_Renderer *renderer, Rectangle rectangle);
void FillBorderedRectangle(SDL_Renderer *renderer, Rectangle rectangle,
                           SDL_Color borderColor = BORDER_BLACK,
                           int thickness = 1);
#endif //RECTANGLE_H