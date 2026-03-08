#include "Animation.h"

void Animation::Load(const char* filepath, int rows, int columns,
    bool bLoop, int startingFrame, int frames)
{
    //Resets on every load since we use the same Animation.h when when iddle and than when dying
    numRows = rows;
    numColumns = columns;
    loop = bLoop;
    startFrame = startingFrame;
    currentFrame = startingFrame;
    bAnimationFinished = false;
    frameTimer = 0.0f;

    //Destroy old texture if replacing animation (swapping to death animation)
    if (textureID) {
        glDeleteTextures(1, &textureID);
        textureID = 0;
    }


    //Magenta is deleted in the shaders
    int width, height, nrChannels;
    stbi_set_flip_vertically_on_load(true);
    unsigned char* data = stbi_load(filepath, &width, &height, &nrChannels, 4); // force RGBA
    if (!data) {
        std::cout << "Animation::Load — failed to open: " << filepath << std::endl;
        return;
    }

    // Remove magenta (255, 0, 255) — same fix as the SDL version
    for (int i = 0; i < width * height * 4; i += 4) {
        if (data[i] == 255 && data[i + 1] == 0 && data[i + 2] == 255) {
            data[i] = data[i + 1] = data[i + 2] = data[i + 3] = 0;
        }
    }

    frameSize = glm::vec2((float)width / columns, (float)height / rows);
    numFrames = (frames == -1) ? (columns * rows) : frames;

    glGenTextures(1, &textureID);
    glBindTexture(GL_TEXTURE_2D, textureID);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

    // Always RGBA since we forced 4 channels above
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height,
        0, GL_RGBA, GL_UNSIGNED_BYTE, data);

    stbi_image_free(data);
    glBindTexture(GL_TEXTURE_2D, 0);
}

void Animation::Update(float deltaTime)
{
    //Dont run animation on finish
    if (bAnimationFinished) return;

    //Update
    frameTimer += deltaTime;
    if (frameTimer >= 1.0f / frameRate) {
        frameTimer = 0.0f;
        ++currentFrame;

        if (currentFrame >= startFrame + numFrames) {
            //loop behaviour
            if (loop)
                currentFrame = startFrame;
            else {
                //holds last frame
                currentFrame = startFrame + numFrames - 1; 
                bAnimationFinished = true;
            }
        }
    }
}

DrawCall Animation::GetDrawCall(const glm::mat4& modelMatrix) const
{
    glm::vec4 uvs = GetFrameUVs();
    DrawCall dc;
    dc.modelMatrix = modelMatrix;
    dc.textureID = textureID;
    dc.uvOffset = glm::vec2(uvs.x, uvs.y);
    dc.uvScale = glm::vec2(uvs.z, uvs.w);
    return dc;
}

glm::vec4 Animation::GetFrameUVs() const
{
    int col = currentFrame % numColumns;
    int row = currentFrame / numColumns;

    float uWidth = 1.0f / numColumns;
    float uHeight = 1.0f / numRows;
    float uOffset = col * uWidth;
    float vOffset = 1.0f - (row + 1) * uHeight; // flipped to match stbi vertical flip

    return glm::vec4(uOffset, vOffset, uWidth, uHeight);
}

void Animation::Cleanup()
{
    if (textureID) {
        glDeleteTextures(1, &textureID);
        textureID = 0;
    }
}