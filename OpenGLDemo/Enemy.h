#pragma once
#include "Character.h"
#include <iostream>
#include "Renderer.h"

//Main reason for its existance so it is easier to manage enemies and enemy missiles if they have the same parent and lets me seperate enemies from player
class Enemy : public Character
{
public:
    //───────────────────────────────────────────────────────────Start setup(in this case is start this inside the enemy manager)───────────────────────────────────────────────────────────
    Enemy(b2WorldId worldId,std::string name, uint16_t id, std::string tag , glm::vec2 position = { 0.0f, 0.0f },
        glm::vec2 scale = { 1.0f, 1.0f }, float rotation = 90.0f, float damage = 10.0f)
        :Character(worldId, "Enemy", 2001, "Enemy", position, scale, rotation, damage)
    {
        SetupHp(25.0f);
        // The child enemy will load another animation inside their constructor this is just for debug porpuses
        LoadAnimation("XenonSprites/LonerA.bmp",4, 4);
    }
};


