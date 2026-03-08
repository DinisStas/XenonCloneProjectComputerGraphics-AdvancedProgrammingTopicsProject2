 #pragma once
#include "Enemy.h"

class Rusher : public Enemy
{
public:
    Rusher( b2WorldId worldId, glm::vec2 position = { 0.0f, 0.0f },
        glm::vec2 scale = { 50.0f, 50.0f }, float rotation = 270.0f, float damage = 10.0f)
        : Enemy(worldId,"Rusher", 2002, "Enemy", position, scale, rotation, damage)
    {
        SetupHp(30.0f);
        LoadAnimation("XenonSprites/Rusher.bmp", 6, 4);
    }
    b2Vec2 enemyVelocity = { -200.0f, 0.0f };

    //Adds here enemy movement
    virtual void UpdatePosition()
    {
        //Makes the previous codes runs
        Enemy::UpdatePosition();
        b2Body_SetLinearVelocity(bodyId, enemyVelocity);
    }
};

