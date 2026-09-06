#pragma once
#include <cmath>

struct Vec2
{
    double x, y;

    Vec2 operator+(Vec2 other) const {return {x + other.x, y + other.y};}
    Vec2 operator-(Vec2 other) const {return {x - other.x, y - other.y};}
    Vec2 operator*(Vec2 other) const {return {x * other.x, y * other.y};}
    Vec2 operator*(double multiplier) const {return {x * multiplier, y * multiplier};}
    Vec2& operator+=(Vec2 other) 
    {
        x += other.x;
        y += other.y;

        return *this;
    }
    Vec2& operator-=(Vec2 other) 
    {
        x -= other.x;
        y -= other.y;

        return *this;
    }
    Vec2& operator*=(Vec2 other) 
    {
        x *= other.x;
        y *= other.y;

        return *this;
    }

    void rotate(double angle);
};