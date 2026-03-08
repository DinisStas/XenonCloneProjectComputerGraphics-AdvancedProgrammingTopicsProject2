#pragma once
#include "Enemy.h"
#include "Player.h"
#include "EnemyMissile.h"

class Loner : public Enemy
{
public:
    Player* target = nullptr;
    Loner(Player* target, b2WorldId worldId, glm::vec2 position = { 0.0f, 0.0f },
        glm::vec2 scale = { 50.0f, 50.0f }, float rotation = 270.0f, float damage = 10.0f)
        :target(target), Enemy(worldId, "Rusher", 2003, "Enemy", position, scale, rotation, damage)
    {
        SetupHp(20.0f);
        LoadAnimation("XenonSprites/LonerA.bmp", 4, 4);
    }
    //In the project 2 it no longer mentions loner movement behvaiour so I'll leave it this way here
    b2Vec2 enemyVelocity = {0.0f, -100.0f };

    //Necessary to fire every 2 seconds
    float fireRate = 2.0f;
    float fireTimer = 0.0f;

    glm::vec2 missileOffset = { -30.0f, 0.0f };
    std::vector<std::unique_ptr<EnemyMissile>> spawnedMissiles;

    //Adds here enemy movement
    void UpdatePosition() override
    {
        //Makes the previous codes runs
        Enemy::UpdatePosition();
        b2Body_SetLinearVelocity(bodyId, enemyVelocity);
    }
    
    void Render(Renderer& renderer, float deltaTime) override
    {
        //Makes the previous codes runs
        Enemy::Render(renderer,deltaTime);

        //Just to confirm that enemy doesnt fire while it is dying
        if (isDead || isDying) return;
       
        //Intercept renderer to run timer inside it 
        fireTimer += deltaTime;
        if (fireTimer >= fireRate) {
            fireTimer = 0.0f;
            FireMissile();
        }
    }


    void FireMissile() {

        // Spawn position is loner position + offset
        glm::vec2 spawnPos = position + missileOffset;

        // Direction from spawn point towards player
        b2Vec2 missileDirection = { -1.0f, 0.0f }; // default left if no target
        if (target)
        {
            glm::vec2 dir = glm::normalize(target->position - spawnPos);
            missileDirection = { dir.x, dir.y };
        }

        //Calculate the angle the missile should be facing based of direction
        float angle = glm::degrees(atan2f(missileDirection.y, missileDirection.x))- 90;

        auto missile = std::make_unique<EnemyMissile>(missileDirection, worldId, spawnPos, glm::vec2(25.0f), angle);
        spawnedMissiles.push_back(std::move(missile));
    }
};


