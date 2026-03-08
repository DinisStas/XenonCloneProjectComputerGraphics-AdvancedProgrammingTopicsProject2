#pragma once
#include "Character.h"
#include "Player.h"

class WeaponPowerUp : public Character
{
public:
    //Wanted to make it a different way but for better implementation reused Die() and
    Player* player;

    WeaponPowerUp(Player* player, b2WorldId worldId, glm::vec2 position = { 0.0f, 0.0f },
        glm::vec2 scale = { 50.0f, 50.0f }, float rotation = 90.0f, float damage = 10.0f)
        :player(player), Character(worldId, "WeaponUpgrade", 2003, "PlayerPowerUp", position, scale, rotation, damage)
    {
        b2Body_SetUserData(bodyId, this);
        LoadAnimation("XenonSprites/PUWeapon.bmp", 2, 4);
    }

    void Die() override {
        if (!player) return; // crash guard
        if (player->currentMissile < 2) player->currentMissile++;
        
        //Dont call the Die character because I don't want the explosion animation on death
        //Disable colliders and stop movement
        b2Body_SetLinearVelocity(bodyId, { 0.0f, 0.0f });
        b2Body_Disable(bodyId);
        isDead = true;
    }

    b2Vec2 velocity = { -100.0f, 0.0f };

    //Adds here movement
    void UpdatePosition() override
    {
        //Makes the previous codes runs
        Character::UpdatePosition();
        b2Body_SetLinearVelocity(bodyId, velocity);
    }
};


