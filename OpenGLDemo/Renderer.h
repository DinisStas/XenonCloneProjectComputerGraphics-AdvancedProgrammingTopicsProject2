#pragma once
#include <glad/glad.h>
#include <glm/glm.hpp>
#include <string>


struct DrawCall {
    glm::mat4   modelMatrix;

    GLuint      textureID = 0;
    glm::vec2   uvOffset = { 0.0f, 0.0f };
    glm::vec2   uvScale = { 1.0f, 1.0f };

    //FallbackColor in case of failure to find texture
    glm::vec4   solidColor = { 0.0f, 0.0f, 0.0f, 1.0f };
};

class Renderer {
public:
    //Start up to load shaders
    void Init(const std::string& vertPath, const std::string& fragPath);

    //Submit a draw call from each character
    void Submit(const DrawCall& call);

    //Sets projection matrix, tried to do in line here did not work
    void SetProjection(const glm::mat4& projection);

    void Cleanup();

private:
    //OpenGlSetup
    GLuint shaderProgram = 0;
    GLuint VAO = 0, VBO = 0, EBO = 0;
    glm::mat4 projection = glm::mat4(1.0f);

    GLuint LoadShaderFromFile(const std::string& path, GLenum type);

    //Set rectangle with part of the spriteSheet to render in open gl
    void SetupQuad();
};