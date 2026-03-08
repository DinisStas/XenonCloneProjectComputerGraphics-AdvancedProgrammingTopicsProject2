#pragma once
#include "Enemy.h"

//Each size has their own spritesheet, The medium and small both have the same speed
enum class AsteroidSize { Large, Medium, Small };

class RockAsteroid : public Enemy
{
public:
    //Depending on the size different spiresheet, speed and die behaviour
    AsteroidSize asteroidSize;

    RockAsteroid(b2WorldId worldId, AsteroidSize size = AsteroidSize::Large ,glm::vec2 position = { 0.0f, 0.0f },
       float rotation = 90.0f, float damage = 20.0f)
        :Enemy(worldId, "Asteroid", 2020, "Enemy", position, GetScale(size), rotation, damage), asteroidSize(size)
    {
        SetupHp(GetHp(size));
        LoadAnimation(GetSprite(size), GetRows(size), GetColumns(size));
    }
    b2Vec2 enemyVelocity = { -100.0f, 0.0f };
    //Adds here checking for player if it is player than damage it
    void UpdatePosition() override
    {
        //scale = GetScale(asteroidSize);
        //Makes the previous codes runs
        Enemy::UpdatePosition();
        b2Body_SetLinearVelocity(bodyId, enemyVelocity);
    }

    //Need this to store all spawned asteroids
    std::vector < std::unique_ptr<RockAsteroid>> spawnedAsteroids;

    void Die() override
    {
        if (isDying) return;
        Character::Die();
        //Return since small asteroids just disapear
        if (asteroidSize == AsteroidSize::Small) return;
        
        //The next spawn size of asteroids L->M->S
        AsteroidSize nextSize;

        if (asteroidSize == AsteroidSize::Large) nextSize = AsteroidSize::Medium;
        else nextSize = AsteroidSize::Small;

        //For some reason scale does not get applied IDK why
        auto asteroidUp = std::make_unique<RockAsteroid>(worldId, nextSize, position + glm::vec2(-20.0f, 40.0f), rotation);
        auto asteroidDown = std::make_unique<RockAsteroid>(worldId, nextSize, position + glm::vec2(-20.0f, -40.0f), rotation);

        asteroidUp->enemyVelocity = { -40.0f,  40.0f };
        asteroidDown->enemyVelocity = { -40.0f, -40.0f };

        spawnedAsteroids.push_back(std::move(asteroidUp));
        spawnedAsteroids.push_back(std::move(asteroidDown));
    }

private:
    static float GetHp(AsteroidSize size)
    {
        switch (size)
        {
        case AsteroidSize::Large:  return 30.0f;
        case AsteroidSize::Medium: return 20.0f;
        case AsteroidSize::Small:  return 10.0f;
        }
        return 30.0f;
    }

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
        case AsteroidSize::Large:  return "XenonSprites/SAster96.bmp";
        case AsteroidSize::Medium: return "XenonSprites/SAster64.bmp";
        case AsteroidSize::Small:  return "XenonSprites/SAster32.bmp";
        }
        return "XenonSprites/SAster96.bmp";
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