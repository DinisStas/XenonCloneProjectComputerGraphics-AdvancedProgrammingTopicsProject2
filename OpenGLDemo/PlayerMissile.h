#pragma once
#include "Character.h"

class PlayerMissile : public Character
{
public:
    //There are 3 missiles types of missile each with 2 frames of animation,this value changes the sprite 0-Light, 2-Medium is 3-Heavy missile
    int currentMissileType = 0;

    PlayerMissile(b2WorldId worldId, glm::vec2 position = { 0.0f, 0.0f },
        glm::vec2 scale = { 1.0f, 1.0f }, float rotation = 270.0f, int missileType = 0)
        : Character(worldId, "Missile", 1002, "Player", position, scale, rotation), currentMissileType(missileType)
    {
        //Using events since using version 3.0 need here to set pointer data for later on get a reference
        b2Body_SetUserData(bodyId, this);
        //Even if I setup the frames manually this sprites still break I don't know why, specially at the end, but sometimes it works
        LoadAnimation("XenonSprites/missile.bmp", 3, 2, true, currentMissileType*2, 2);
        SetupHp(1.0f);
        if (currentMissileType != 0)
        {
            damage = currentMissileType * damage;
        }
    }

    //Speed
    b2Vec2 missileVelocity = { 200.0f, 0.0f};

    void UpdatePosition()
    {
        //Makes sure the physic boxes follow the enemy
        Character::UpdatePosition();
        b2Body_SetLinearVelocity(bodyId, missileVelocity);
    }
};

