#include "DebugDraw.h"
#include <iostream>

static const char* vertSrc = R"(
#version 330 core
layout(location = 0) in vec2 aPos;
uniform mat4 projection;
void main() {
    gl_Position = projection * vec4(aPos, 0.0, 1.0);
}
)";

static const char* fragSrc = R"(
#version 330 core
uniform vec3 color;
out vec4 FragColor;
void main() {
    FragColor = vec4(color, 1.0);
}
)";

void DebugDraw::Init()
{
    shaderProgram = CompileShader(vertSrc, fragSrc);
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, 16 * sizeof(float), nullptr, GL_DYNAMIC_DRAW);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    glBindVertexArray(0);
}

void DebugDraw::DrawBody(b2BodyId bodyId, const glm::mat4& projection, glm::vec3 color)
{
    if (!b2Body_IsValid(bodyId)) return;

    // Get all shapes on this body — max 8 is safe for our use case
    b2ShapeId shapes[8];
    int shapeCount = b2Body_GetShapes(bodyId, shapes, 8);

    b2Transform transform = b2Body_GetTransform(bodyId);

    for (int i = 0; i < shapeCount; i++)
    {
        if (b2Shape_GetType(shapes[i]) != b2_polygonShape) continue;

        b2Polygon poly = b2Shape_GetPolygon(shapes[i]);

        // Transform each vertex from local body space to world space
        b2Vec2 worldVerts[8];
        for (int v = 0; v < poly.count; v++)
            worldVerts[v] = b2TransformPoint(transform, poly.vertices[v]);

        // Sensors draw green, solid shapes draw red unless color overridden
        glm::vec3 drawColor = b2Shape_IsSensor(shapes[i])
            ? glm::vec3(0.0f, 1.0f, 0.0f)
            : color;

        DrawPolygon(worldVerts, poly.count, projection, drawColor);
    }
}

void DebugDraw::DrawPolygon(const b2Vec2* vertices, int count,
    const glm::mat4& projection, glm::vec3 color)
{
    float verts[16]; // 8 vertices max * 2 floats
    for (int i = 0; i < count; i++) {
        verts[i * 2] = vertices[i].x;
        verts[i * 2 + 1] = vertices[i].y;
    }

    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferSubData(GL_ARRAY_BUFFER, 0, count * 2 * sizeof(float), verts);

    glUseProgram(shaderProgram);
    glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "projection"),
        1, GL_FALSE, glm::value_ptr(projection));
    glUniform3f(glGetUniformLocation(shaderProgram, "color"),
        color.r, color.g, color.b);

    glDrawArrays(GL_LINE_LOOP, 0, count);
    glBindVertexArray(0);
    glUseProgram(0);
}

void DebugDraw::Cleanup()
{
    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
    glDeleteProgram(shaderProgram);
}

GLuint DebugDraw::CompileShader(const char* vert, const char* frag)
{
    auto compile = [](const char* src, GLenum type) -> GLuint {
        GLuint id = glCreateShader(type);
        glShaderSource(id, 1, &src, nullptr);
        glCompileShader(id);
        int ok; char log[512];
        glGetShaderiv(id, GL_COMPILE_STATUS, &ok);
        if (!ok) { glGetShaderInfoLog(id, 512, nullptr, log); std::cout << log << std::endl; }
        return id;
        };
    GLuint v = compile(vert, GL_VERTEX_SHADER);
    GLuint f = compile(frag, GL_FRAGMENT_SHADER);
    GLuint prog = glCreateProgram();
    glAttachShader(prog, v);
    glAttachShader(prog, f);
    glLinkProgram(prog);
    glDeleteShader(v);
    glDeleteShader(f);
    return prog;
}