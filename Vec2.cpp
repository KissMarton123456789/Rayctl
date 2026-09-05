#include "Vec2.hpp"

void Vec2::rotate(double angle)
{
    double oldX = x;
    x = x * std::cos(angle) - y * std::sin(angle);
    y = oldX * std::sin(angle) - y * std::cos(angle);
}