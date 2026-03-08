#pragma once
#include "Character.h"
#include <glad/glad.h>
#include <glm/gtc/type_ptr.hpp>
#include "Renderer.h"
#include "PlayerMissile.h"
#include "HealthBar.h"

class Player : public Character {
public:
    Player(b2WorldId worldId, glm::vec2 position = { 0.0f, 0.0f },
        glm::vec2 scale = { 1.0f, 1.0f }, float rotation = 270.0f, float damage= 1000.0f,
        float worldWidth = 1200.0f, float worldHeight = 800.0f)
        : Character(worldId, "PlayerCharacter", 1001, "Player", position, scale, rotation, damage), width(worldWidth), height(worldHeight)
    {
        SetupHp(100.0f);
        //Making this inside the character for some reason causes issues further on when the game is running
        //Temporary
        LoadAnimation("XenonSprites/Ship1.bmp", 1, 7, true , 4, 1);
    }

    //Max 3 lifes
    float lifes = 1;

    //Current missile equiped by the player changes animation and damage of the missles
    int currentMissile = 0;

    std::vector<std::unique_ptr<PlayerMissile>> missiles;

    //Fires missiles
    void FireMissile() { 
        if (fireTimer >= fireRate)
        {
            fireTimer = 0.0f;
            glm::vec2 offset = glm::vec2(20.0f, 0);
            auto missile = std::make_unique<PlayerMissile>(worldId, position + offset, glm::vec2(25.0f), 270.f, currentMissile);
            missiles.push_back(std::move(missile));
            std::cout << "Fired Missile inside player " << missiles.size() << std::endl;
        }
    }

    //This override so the player doesn't leave the map bounds
    //!!! this is hard coded in term of bounds
    void UpdatePosition() override
    {
        Character::UpdatePosition();

        //Takes player size into account when clampin
        float halfW = scale.x * 0.4f;
        float halfH = scale.y * 0.4f;

        //Clamp position within the map bounds
        position.x = glm::clamp(position.x, halfW, width - halfW);
        position.y = glm::clamp(position.y, halfH, height - halfH);

        //Push the Box2D body to the clamped position so physics stays in sync
        b2Body_SetTransform(bodyId, { position.x, position.y }, b2Body_GetRotation(bodyId));
    }

    HealthBar healthBar;

    void Render(Renderer& renderer, float deltaTime) override {

        Character::Render(renderer, deltaTime);
        healthBar.Render(renderer, getHpPercent(), (int)lifes, deltaTime);

        fireTimer += deltaTime;

        for (auto i = missiles.begin(); i != missiles.end(); )
        {
            if ((*i)->isDead)  // only erase after death animation finished
                i = missiles.erase(i);
            else
            {
                (*i)->Render(renderer, deltaTime);
                ++i;
            }
        }
    }
    //Player has lifes so doesnt die on first death
    void Die() override {
        if (isDying) return;
 
        std::cout << "Player Die " << missiles.size() << std::endl;

        if (lifes > 0) {
            lifes--;
            //There is no overheal so this value poses no problem, tough I could just get HpMax
            HpModify(1000);
            std::cout << "Player life " << missiles.size() << std::endl;
        }
        else Character::Die(); // if no life left just die characcter
    }

private:
    float width;
    float height;

    //Timer used to measure when to fire again
    float fireTimer = 0.0f;
    //Yes I know its not the best implementation to have the fireRate inside the player
    float fireRate = 0.5f;

};