#pragma once
#include "Enemy.h"

class Drone : public Enemy
{
public:
    Drone(b2WorldId worldId, glm::vec2 position = { 0.0f, 0.0f },
        glm::vec2 scale = { 50.0f, 50.0f }, float rotation = 90.0f, float damage = 10.0f)
        : Enemy(worldId, "Drone", 2015, "Enemy", position, scale, rotation, damage)
    {
        SetupHp(10.0f);
        LoadAnimation("XenonSprites/Drone.bmp", 2, 8);
    }
    b2Vec2 enemyVelocity = { 0.0f, 0.0f };
    //Timer for cos animation
    float timer = 0;

    //I don't think its the best aproach but in this situtation I don't think its that bad???
    void UpdatePosition () override
    {
        //Makes the previous codes runs
        Enemy::UpdatePosition();
      
        enemyVelocity = { -150.0f, cos(timer * 2) *120 };//cos(rate)*Amplitude--- I did not setup the variables but in this case rate changes the speed of oscilations while amplitude changes its size yay trygonometry 
        b2Body_SetLinearVelocity(bodyId, enemyVelocity);
    }

    void Render(Renderer& renderer, float deltaTime) override
    {
        //Makes the previous codes runs
        Enemy::Render(renderer, deltaTime);

        //Intercept renderer input to run timer inside it 
        timer += deltaTime;
    }

};


