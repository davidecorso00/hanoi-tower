#pragma once

#include "Lib.h"
#include "Object.h"

#include <string>

/**
 * @brief Class representing a texture.
 *
 * Inherits from Object. Stores file path, generates an OpenGL texture ID,
 * and provides a render method to bind the texture.
 */
class ENG_API Texture : public Object {
public:
    /**
     * @brief Constructor that takes a name and file path.
     * @param name Name of the texture object.
     * @param filepath Path to the image file.
     */
    Texture(const std::string& textureObjectName, const std::string& imageFilePath);

    /**
     * @brief Destructor. Deletes the OpenGL texture if allocated.
     */
    ~Texture();

    /**
     * @brief Binds the texture in OpenGL.
     *        Must be called before drawing geometry using this texture.
     */
    void render() override;

private:
    unsigned int textureId;
    std::string imageFilePath;
};