#include "Player.hpp"

void Player::turn(double angle)
{
    dir.rotate(angle);
    plane.rotate(angle);
}