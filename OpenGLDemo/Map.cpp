#include "Map.h"
#include <glm/gtc/matrix_transform.hpp>


void Map::Init(Renderer& renderer,
    const std::string& bgTexPath, float bgScrollSpeed,
    const std::string& fgTexPath, float fgScrollSpeed,
    float windowW, float windowH)
{
    windowWidth = windowW;
    windowHeight = windowH;

    layers[0].textureID = LoadTexture(bgTexPath);
    layers[0].scrollSpeed = bgScrollSpeed;
    layers[0].width = windowW;
    layers[0].height = windowH;
    layers[0].scrollOffset = 0.0f;

    layers[1].textureID = LoadTexture(fgTexPath);
    layers[1].scrollSpeed = fgScrollSpeed;
    layers[1].width = windowW;
    layers[1].height = windowH;
    layers[1].scrollOffset = 0.0f;
}

void Map::Cleanup()
{
    for (int i = 0; i < 2; i++) {
        if (layers[i].textureID)
            glDeleteTextures(1, &layers[i].textureID);
    }
}
void Map::Update(float deltaTime)
{
    //Currently only 2 layers
    for (int i = 0; i < 2; i++)
    {
        //Reset on full scroll
        layers[i].scrollOffset += layers[i].scrollSpeed * deltaTime;
        if (layers[i].scrollOffset >= layers[i].width)
            layers[i].scrollOffset = 0.0f;
    }
}

void Map::Render(Renderer& renderer, float deltaTime)
{
    Update(deltaTime);

    float halfW = windowWidth * 0.5f;
    float halfH = windowHeight * 0.5f;

    //Has 2 scroll tiles so there won't be a moment where the background is offscreen done this way since in the project instead of having a long repeatable background we have reletively small images
    for (int i = 0; i < 2; i++)
    {
        const MapLayer& layer = layers[i];

        //Tile 1 scroll
        glm::mat4 model = glm::mat4(1.0f);
        model = glm::translate(model, glm::vec3(halfW - layer.scrollOffset, halfH, 0.0f));
        model = glm::scale(model, glm::vec3(layer.width, layer.height, 1.0f));
        DrawCall dc;
        dc.modelMatrix = model;
        dc.textureID = layer.textureID;
        dc.uvOffset = { 0.0f, 0.0f };
        dc.uvScale = { 1.0f, 1.0f };
        renderer.Submit(dc);

        //Tile 2 scroll
        glm::mat4 model2 = glm::mat4(1.0f);
        model2 = glm::translate(model2, glm::vec3(halfW - layer.scrollOffset + layer.width, halfH, 0.0f));
        model2 = glm::scale(model2, glm::vec3(layer.width, layer.height, 1.0f));
        DrawCall dc2;
        dc2.modelMatrix = model2;
        dc2.textureID = layer.textureID;
        dc2.uvOffset = { 0.0f, 0.0f };
        dc2.uvScale = { 1.0f, 1.0f };
        renderer.Submit(dc2);
    }
}

GLuint Map::LoadTexture(const std::string& path)
{
    //Using LoadTexture from another script caused issues when moving to openGl
    int w, h, ch;
    stbi_set_flip_vertically_on_load(true);
    unsigned char* data = stbi_load(path.c_str(), &w, &h, &ch, 4);
    if (!data) {
        printf("Map: failed to load texture: %s\n", path.c_str());
        return 0;
    }
    GLuint id;
    glGenTextures(1, &id);
    glBindTexture(GL_TEXTURE_2D, id);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, w, h, 0,
        GL_RGBA, GL_UNSIGNED_BYTE, data);
    glGenerateMipmap(GL_TEXTURE_2D);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    stbi_image_free(data);
    return id;
}
