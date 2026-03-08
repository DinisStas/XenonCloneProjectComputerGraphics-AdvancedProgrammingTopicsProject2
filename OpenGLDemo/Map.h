#pragma once
#include "Renderer.h"
#include <glad/glad.h>
#include <glm/glm.hpp>
#include <box2d/box2d.h>
#include "stb_image.h"
#include <string>

struct MapLayer {
    GLuint  textureID = 0;
    float   scrollSpeed = 1.0f;
    float   width = 0.0f;
    float   height = 0.0f;
    float   scrollOffset = 0.0f;
};

class Map {
public:
    void Init(Renderer& renderer,
        const std::string& bgTexPath, float bgScrollSpeed,
        const std::string& fgTexPath, float fgScrollSpeed,
        float windowW, float windowH);

    void Update(float deltaTime);
    void Render(Renderer& renderer, float deltaTime);

    void Cleanup();

private:
    //background layers
    MapLayer layers[2];
    float windowWidth = 0.f;
    float windowHeight = 0.f;
    //Might have been possible to reuse the one from animation but it caused a buggy mess
    GLuint LoadTexture(const std::string& path);
};