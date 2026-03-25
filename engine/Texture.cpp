#include "Texture.h"

#include <iostream>

#include <GL/freeglut.h>
#include <FreeImage.h>

/**
*
* STEPS:
* 1. Create a texture handle
* 2. Determine image format via FreeImage
* 3. Convert to 32-bit format (BGRA → needed for OpenGL)
* 
*/

Texture::Texture(const std::string& textureObjectName, const std::string& imageFilePath)
    : Object(textureObjectName), imageFilePath(imageFilePath), textureId (0)
{
    // 1
    glGenTextures(1, &textureId);
    glBindTexture(GL_TEXTURE_2D, textureId);

    // 2
    FREE_IMAGE_FORMAT imgFormat = FreeImage_GetFileType(imageFilePath.c_str(), 0);

    // - Try inferring from file extension if unknown
    if (imgFormat == FIF_UNKNOWN) {
        imgFormat = FreeImage_GetFIFFromFilename(imageFilePath.c_str());
    }

    if (imgFormat == FIF_UNKNOWN) {
        std::cerr << "[Texture] Unsupported or unrecognized file format: "
            << imageFilePath << std::endl;
        glBindTexture(GL_TEXTURE_2D, 0);
        return;
    }

    // Load bitmap data into memory
    FIBITMAP* loadedBitmap = FreeImage_Load(imgFormat, imageFilePath.c_str());
    if (!loadedBitmap) {
        std::cerr << "[Texture] Failed to load image: " << imageFilePath << std::endl;
        glBindTexture(GL_TEXTURE_2D, 0);
        return;
    }

    // 3
    FIBITMAP* rgbaImage = FreeImage_ConvertTo32Bits(loadedBitmap);
    FreeImage_Unload(loadedBitmap);

    if (!rgbaImage) {
        std::cerr << "[Texture] Failed to convert image to 32-bit RGBA: "
            << imageFilePath << std::endl;
        glBindTexture(GL_TEXTURE_2D, 0);
        return;
    }

    // Retrieve image dimensions
    const int imgWidth = FreeImage_GetWidth(rgbaImage);
    const int imgHeight = FreeImage_GetHeight(rgbaImage);

    // Upload data to GPU memory
    glTexImage2D(
        GL_TEXTURE_2D,
        0,
        GL_RGBA,
        imgWidth,
        imgHeight,
        0,
        GL_BGRA_EXT,
        GL_UNSIGNED_BYTE,
        FreeImage_GetBits(rgbaImage)
    );

    // Basic OpenGL texture parameters
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

    std::cout << "[Texture] Loaded texture '" << imageFilePath
        << "' with ID " << textureId << std::endl;

    FreeImage_Unload(rgbaImage);

    // Unbind texture after initialization
    glBindTexture(GL_TEXTURE_2D, 0);
}

Texture::~Texture() {
    if (textureId != 0) { glDeleteTextures(1, &textureId); }
}

void Texture::render() {
    if (textureId != 0) { glBindTexture(GL_TEXTURE_2D, textureId); }
}