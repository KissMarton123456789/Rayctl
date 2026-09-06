#include "Player.hpp"

void Player::turn(double angle)
{
    angle = -angle;
    double oldDirX = dir.x;
    double oldPlaneX = plane.x;

    dir.x = dir.x * std::cos(angle) - dir.y * std::sin(angle);
    dir.y = oldDirX * std::sin(angle) + dir.y * std::cos(angle); 

    plane.x = plane.x * std::cos(angle) - plane.y * std::sin(angle);
    plane.y = oldPlaneX * std::sin(angle) + plane.y * std::cos(angle);
}