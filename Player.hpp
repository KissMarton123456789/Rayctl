 #include "Vec2.hpp"
 #pragma once

struct Player
{
    Vec2 pos{22.0, 12.0};
    Vec2 dir{-1.0, 0.0};
    Vec2 plane{0.0, 0.66};

    bool isMovingForward;
    bool isMovingBackward;
    bool isMovingLeft;
    bool isMovingRight;

    void turn(double angle);
};