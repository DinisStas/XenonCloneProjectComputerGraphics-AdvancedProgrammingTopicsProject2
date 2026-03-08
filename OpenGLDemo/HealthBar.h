#pragma once
#include <glm/glm.hpp>
#include "Renderer.h"
#include "HpInterface.h"
#include "Animation.h"
#include <glm/gtc/matrix_transform.hpp>


class HealthBar
{
public:
    glm::vec2 position = { 200.0f, 750.0f };
    glm::vec2 size = { 300.0f, 20.0f };

    // Life icon size and spacing
    glm::vec2 lifeIconSize = { 30.0f, 30.0f };
    float lifeIconSpacing = 35.0f;
    glm::vec2 lifeIconOffset = { 0.0f, -40.0f }; // above the health bar

    //Horrible implementation I know should have made something separeta for Ui instead of taking sprite from animation
    Animation lifeIcon;
    HealthBar()
    {
        //Horrible implementation indeed
        lifeIcon.Load("XenonSprites/Ship1.bmp", 1, 7, true, 4, 1);
    }

    //Gets rendered inside player since it provide the player provides the values
    void Render(Renderer& renderer, float hpPercent, int lives, float deltaTime)
    {
        // Health bar background
        DrawQuad(renderer, position, size, { 0.2f, 0.2f, 0.2f, 1.0f });
        // Health bar fill
        glm::vec2 fillSize = { size.x * hpPercent, size.y };
        glm::vec2 fillPos = { position.x - (size.x - fillSize.x) * 0.5f, position.y };
        DrawQuad(renderer, fillPos, fillSize, GetColor(hpPercent));

        // Draw life icons above the bar
        lifeIcon.Update(deltaTime);
        glm::vec2 iconStart = { position.x - size.x * 0.5f, position.y + lifeIconOffset.y };
        for (int i = 0; i < lives; i++)
        {
            glm::vec2 iconPos = { iconStart.x + i * lifeIconSpacing, iconStart.y };
            glm::mat4 model = glm::mat4(1.0f);
            model = glm::translate(model, glm::vec3(iconPos, 0.0f));
            model = glm::rotate(model, glm::radians(270.0f), glm::vec3(0.0f, 0.0f, 1.0f));
            model = glm::scale(model, glm::vec3(lifeIconSize, 1.0f));
            renderer.Submit(lifeIcon.GetDrawCall(model));
        }
    }

private:
    glm::vec4 GetColor(float pct)
    {
        if (pct > 0.5f) return { 0.15f, 0.85f, 0.15f, 1.0f };
        if (pct > 0.3f) return { 1.0f,  0.75f, 0.0f,  1.0f };
        return { 0.9f,   0.0f,  0.0f,  1.0f };
    }

    void DrawQuad(Renderer& renderer, glm::vec2 pos, glm::vec2 sz, glm::vec4 color)
    {
        glm::mat4 model = glm::mat4(1.0f);
        model = glm::translate(model, glm::vec3(pos, 0.0f));
        model = glm::scale(model, glm::vec3(sz, 1.0f));
        DrawCall dc;
        dc.modelMatrix = model;
        dc.solidColor = color;
        renderer.Submit(dc);
    }
};