#pragma once
#include "GameObject.h"
#include "HpInterface.h"
#include "Animation.h"
#include "box2d/box2d.h"

class Character : public GameObject, public HpInterface{
public:
    //Unfornetly can't to replace the id of the character with bodyId since its a struct with 3ints
    b2BodyId bodyId;
    b2WorldId worldId;
    //Damage a character deal to another character with a different tag on contact
    float damage;

    Renderer* renderer;

    Character(b2WorldId worldId, std::string name, uint16_t id, std::string tag, glm::vec2 position, glm::vec2 scale, float rotation, float damage=10.0f) :worldId(worldId)//Need to seperate world id since there is not world Id inside GameObject
        , GameObject(name, id, tag, position, scale, rotation), damage(damage) { SetupCollider(); };
     
  
    //───────────────────────────────────────────────────────────Collisions and Positioning───────────────────────────────────────────────────────────

    //b2Body NEEDS to be deleted or cause error in the manager if not deleted
    ~Character()
    {
        if (b2Body_IsValid(bodyId)) b2DestroyBody(bodyId);
    }

    //Left the function here since I dont plan on creating a cpp just for 2 setup functions also both public since a child might want to alter the values
    virtual void SetupCollider()
    {
        //Creating a dinamic body
        b2BodyDef bodyDef = b2DefaultBodyDef();
        bodyDef.type = b2_dynamicBody;

        //Setting up current position (would not accept the vec2 position as is so had to get each coordinate individually)
        bodyDef.position = { position.x, position.y };
        bodyDef.rotation = b2MakeRot(glm::radians(rotation));
        bodyId = b2CreateBody(worldId, &bodyDef);

        b2Body_SetUserData(bodyId, this);

        //Currently a box shape might modify it !!!!!
        b2Polygon  boxShape1 = b2MakeBox(scale.x * 0.5f, scale.y * 0.5f);
        //If I want a more complex box shaped collision this would do it but IDK
        //b2Polygon  boxShape = b2MakeRoundedBox(scale.x * 0.4f, scale.y * 0.4f, scale.x * 0.1f);
        b2ShapeDef shapeDef = b2DefaultShapeDef();
        shapeDef.density = 1.0f;

        //This one detects overlaps only
        shapeDef.isSensor = true;
        shapeDef.enableSensorEvents = true; 
        b2CreatePolygonShape(bodyId, &shapeDef, &boxShape1);
    }

    //Makes it so the position of the character and box are synced
    virtual void UpdatePosition()
    {
        b2Vec2 physicsPos = b2Body_GetPosition(bodyId);
        position = { physicsPos.x, physicsPos.y };
    }

    //───────────────────────────────────────────────────────────DEATH───────────────────────────────────────────────────────────

    bool isDead = false;
    bool isDying = false;
    bool IsDead() const { return isDead; }


    virtual void Die() override {
        std::cout << "Character object - " << this->getName() << " has died" << std::endl;
        if (isDying) return;
        isDying = true;

        //Disable collidrs and stop movement
        b2Body_SetLinearVelocity(bodyId, { 0.0f, 0.0f });
        b2Body_Disable(bodyId);

        //Load death animation when it finishes render will confirm if player is dead
        LoadAnimation("XenonSprites/explode64.bmp", 2, 5, false);
    }


    //───────────────────────────────────────────────────────────Animation and Rendering───────────────────────────────────────────────────────────

    Animation animation;
    //Execute this inside the setup of the child to load the texture inside the animator
    void LoadAnimation(const char* filepath, int rows, int columns, bool bLoop = true, int startingFrame = 0, int frames = -1)
    {
        animation.Load(filepath, rows, columns, bLoop, startingFrame, frames);
    }

    virtual void Render(Renderer& renderer, float deltaTime)
    {
        //Update physics and animation
        UpdatePosition();
        animation.Update(deltaTime);

        glm::mat4 model = glm::mat4(1.0f);

        model = glm::translate(model, glm::vec3(position, 0.0f));
        model = glm::rotate(model, glm::radians(rotation), glm::vec3(0.0f, 0.0f, 1.0f));
        model = glm::scale(model, glm::vec3(scale, 1.0f));

        //Submitnit to draw
        if (animation.textureID != 0) renderer.Submit(animation.GetDrawCall(model));
        else
        {
            //If no texture is found replace sprite with a solid color
            DrawCall dc;
            dc.modelMatrix = model;
            dc.solidColor = { 1.0f, 0.4f, 0.2f, 1.0f }; // red placeholder
            renderer.Submit(dc);
        }

        if (isDying && animation.bAnimationFinished) isDead = true;
    }
};