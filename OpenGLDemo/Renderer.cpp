#include "Renderer.h"
#include <glm/gtc/type_ptr.hpp>
#include <fstream>
#include <sstream>
#include <iostream>

void Renderer::Init(const std::string& vertPath, const std::string& fragPath)
{
    GLuint vert = LoadShaderFromFile(vertPath, GL_VERTEX_SHADER);
    GLuint frag = LoadShaderFromFile(fragPath, GL_FRAGMENT_SHADER);

    shaderProgram = glCreateProgram();
    glAttachShader(shaderProgram, vert);
    glAttachShader(shaderProgram, frag);
    glLinkProgram(shaderProgram);

    int success; char log[512];
    glGetProgramiv(shaderProgram, GL_LINK_STATUS, &success);
    if (!success) {
        glGetProgramInfoLog(shaderProgram, 512, nullptr, log);
        std::cout << "Renderer link error: " << log << std::endl;
    }

    glDeleteShader(vert);
    glDeleteShader(frag);
    SetupQuad();
}

void Renderer::SetProjection(const glm::mat4& proj)
{
    projection = proj;
}

void Renderer::Submit(const DrawCall& call)
{
    glUseProgram(shaderProgram);

    // Upload matrices
    glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "model"),1, GL_FALSE, glm::value_ptr(call.modelMatrix));
    glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "projection"), 1, GL_FALSE, glm::value_ptr(projection));

    if (call.textureID != 0) {
        //Animated sprite path
        glUniform1i(glGetUniformLocation(shaderProgram, "useTexture"), 1);
        glUniform2f(glGetUniformLocation(shaderProgram, "uvOffset"),
            call.uvOffset.x, call.uvOffset.y);
        glUniform2f(glGetUniformLocation(shaderProgram, "uvScale"),
            call.uvScale.x, call.uvScale.y);
        glUniform1i(glGetUniformLocation(shaderProgram, "spriteSheet"), 0);
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, call.textureID);
    }
    else {
        //Solid color fallback
        glUniform1i(glGetUniformLocation(shaderProgram, "useTexture"), 0);
        glUniform4f(glGetUniformLocation(shaderProgram, "solidColor"),
            call.solidColor.r, call.solidColor.g,
            call.solidColor.b, call.solidColor.a);
    }

    //BindVAO
    glBindVertexArray(VAO);
    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
    glBindVertexArray(0);
    glUseProgram(0);
}

void Renderer::Cleanup()
{
    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
    glDeleteBuffers(1, &EBO);
    glDeleteProgram(shaderProgram);
}

void Renderer::SetupQuad()
{
    //This is done in a way to match the box 2D center
    float vertices[] = {
        // pos          // uv
        -0.5f,  0.5f,  0.0f, 1.0f,  // top-left
         0.5f,  0.5f,  1.0f, 1.0f,  // top-right
         0.5f, -0.5f,  1.0f, 0.0f,  // bottom-right
        -0.5f, -0.5f,  0.0f, 0.0f,  // bottom-left
    };
    unsigned int indices[] = { 0, 1, 2,  0, 2, 3 };

    //Generate all open gl necesseties and assign vertices correctly
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);

    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);


    //My vertex shaders allow for this
    //Position (location 0)
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    //Uv (location 1)
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)(2 * sizeof(float)));
    glEnableVertexAttribArray(1);

    glBindVertexArray(0);
}

GLuint Renderer::LoadShaderFromFile(const std::string& path, GLenum type)
{
    //Sinc I no longet have a shader class I do it this way
    std::ifstream file(path);
    if (!file.is_open()) {
        std::cout << "Failed to open shader: " << path << std::endl;
        return 0;
    }
    std::stringstream ss;
    ss << file.rdbuf();
    std::string src = ss.str();
    const char* cstr = src.c_str();

    GLuint id = glCreateShader(type);
    glShaderSource(id, 1, &cstr, nullptr);
    glCompileShader(id);

    int success; char log[512];
    glGetShaderiv(id, GL_COMPILE_STATUS, &success);
    if (!success) {
        glGetShaderInfoLog(id, 512, nullptr, log);
        std::cout << "Shader compile error (" << path << "): " << log << std::endl;
    }
    return id;
}