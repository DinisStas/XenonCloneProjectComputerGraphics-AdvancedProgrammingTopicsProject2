#pragma once
#include "Enemy.h"
#include "RockAsteroid.h"

//Probably would have been better if Rock asteroid inherited from this class but override the Die and the spawn asteroid logic but I did the rock asteroid first 
//Same as Rock asteroid but does not die
class MetalAsteroid : public Enemy
{
public:
    //Depending on the size different spiresheet, speed and die behaviour
    AsteroidSize asteroidSize;

    MetalAsteroid(b2WorldId worldId, AsteroidSize size = AsteroidSize::Large, glm::vec2 position = { 0.0f, 0.0f },
        float rotation = 90.0f, float damage = 10.0f)
        :Enemy(worldId, "MetalAsteroid", 2021, "Enemy", position, GetScale(size), rotation, damage), asteroidSize(size)
    {
        LoadAnimation(GetSprite(size), GetRows(size), GetColumns(size));
    }
    b2Vec2 enemyVelocity = { -100.0f, 0.0f };
    //Adds here checking for player if it is player than damage it
    virtual void UpdatePosition()
    {
        //Makes the previous codes runs
        Enemy::UpdatePosition();
        b2Body_SetLinearVelocity(bodyId, enemyVelocity);
    }

    void Die() override
    {
        return;
    }

private:
    static glm::vec2 GetScale(AsteroidSize size)
    {
        switch (size)
        {
        case AsteroidSize::Large:  return { 100.0f, 100.0f };
        case AsteroidSize::Medium: return { 50.0f, 50.0f };
        case AsteroidSize::Small:  return { 25.0f, 25.0f };
        }
        return { 50.0f, 50.0f };
    }

    static const char* GetSprite(AsteroidSize size)
    {
        switch (size)
        {
        case AsteroidSize::Large:  return "XenonSprites/MAster96.bmp";
        case AsteroidSize::Medium: return "XenonSprites/MAster64.bmp";
        case AsteroidSize::Small:  return "XenonSprites/MAster32.bmp";
        }
        return "XenonSprites/MAster96.bmp";
    }

    static int GetRows(AsteroidSize size)
    {
        switch (size)
        {
        case AsteroidSize::Large:  return 5;
        case AsteroidSize::Medium: return 3;
        case AsteroidSize::Small:  return 2;
        }
        return 5;
    }

    static int GetColumns(AsteroidSize size)
    {
        switch (size)
        {
        case AsteroidSize::Large:  return 5;
        case AsteroidSize::Medium: return 8;
        case AsteroidSize::Small:  return 8;
        }
        return 5;
    }
};