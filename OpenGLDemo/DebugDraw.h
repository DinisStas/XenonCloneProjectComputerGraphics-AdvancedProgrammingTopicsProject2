#pragma once
#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <box2d/box2d.h>
#include <vector>

//This was not done by me
class DebugDraw {
public:
    void Init();
    void DrawBody(b2BodyId bodyId, const glm::mat4& projection, glm::vec3 color = { 0.0f, 1.0f, 0.0f });
    void Cleanup();

private:
    GLuint VAO = 0, VBO = 0;
    GLuint shaderProgram = 0;

    void DrawPolygon(const b2Vec2* vertices, int count, const glm::mat4& projection, glm::vec3 color);
    GLuint CompileShader(const char* vert, const char* frag);
};