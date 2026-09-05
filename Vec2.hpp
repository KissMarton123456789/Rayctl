#pragma once
#include <cmath>

struct Vec2
{
    double x, y;

    Vec2 operator+(Vec2 other) const {return {x + other.x, y + other.y};}
    Vec2 operator-(Vec2 other) const {return {x - other.x, y - other.y};}
    Vec2 operator*(Vec2 other) const {return {x * other.x, y * other.y};}

    void rotate(double angle);
};