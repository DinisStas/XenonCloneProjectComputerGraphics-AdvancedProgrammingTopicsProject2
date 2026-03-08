#pragma once
#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include "stb_image.h"
#include "Renderer.h"
#include <iostream>
#include <string>

class Animation {
public:
    GLuint    textureID = 0;
    glm::vec2 frameSize = { 0.0f, 0.0f };
    int  numFrames = 0;
    int  startFrame = 0;
    int  numRows = 0;
    int  numColumns = 0;
    int  currentFrame = 0;
    float frameTimer = 0.0f;
    float frameRate = 12.0f;
    bool  loop = true;
    bool  bAnimationFinished = false;

    //Calculates frames and their size based on the rows and columns provided
    void Load(const char* filepath, int rows, int columns,
        bool bLoop = true, int startingFrame = 0, int frames = -1);

    void Update(float deltaTime);

    //Returns a DrawCall built from screen-space position and size
    DrawCall GetDrawCall(const glm::mat4& modelMatrix) const;

    void Cleanup();

private:
    glm::vec4 GetFrameUVs() const;
};