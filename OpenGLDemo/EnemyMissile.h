#pragma once
#include "Enemy.h"

class EnemyMissile : public Enemy
{
public:
    //Direction given for the enemy to go towards the player needs to be normalized before being provided
    b2Vec2 missileDirection;

    EnemyMissile(b2Vec2 missileDirection, b2WorldId worldId, glm::vec2 position = { 0.0f, 0.0f },
        glm::vec2 scale = { 1.0f, 1.0f }, float rotation = 270.0f, float damage = 10.0f)
        :missileDirection(missileDirection), Enemy(worldId, "EnemyMissile", 2050, "Enemy", position, scale, rotation, damage)
    {
        //Using events since using version 3.0 need here to set pointer data for later on get a reference
        b2Body_SetUserData(bodyId, this);
        LoadAnimation("XenonSprites/missile.bmp", 3, 2,true , 0, 2);
        SetupHp(1.0f);
    }
private:

    //Speed of the missile
    float missileSpeed = 200.0f  ;
    //Direction and speed
    b2Vec2 missileVelocity = missileSpeed * missileDirection;

    void UpdatePosition() override
    {
        //Makes sure the physic boxes follow the Enemy
        Character::UpdatePosition();
        b2Body_SetLinearVelocity(bodyId, missileVelocity);
    }

};

