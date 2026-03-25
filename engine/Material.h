#pragma once

#include "Lib.h"

#include "Object.h"
#include "Texture.h"

#include "glm/glm.hpp"

/**
 * @brief Represents a material applied to a mesh in the scene.
 *
 * Contains its properties.
 */
class ENG_API Material : public Object {
public:
    /**
     * @brief Full constructor initializing all properties.
     * @param name Name of the material.
     * @param ambient Ambient color.
     * @param diffuse Diffuse color.
     * @param emission Emission color.
     * @param shininess Specular exponent.
     * @param specular Specular color.
     * @param transparency Transparency factor (0 = opaque, 1 = fully transparent).
     */
    Material(
        const std::string& materialName,
        const glm::vec3& emissionColor,
        const glm::vec3& ambientColor,
        const glm::vec3& diffuseColor,
        const glm::vec3& specularColor,
        float shininessExponent,
        float transparencyFactor
    );

    /**
     * @brief Destructor.
    */
    ~Material();

    // -------------------
    // Getters
    // -------------------
    const glm::vec3& getEmissionColor() const;      /**< Returns the emission color. */
    const glm::vec3& getDiffuseColor() const;       /**< Returns the diffuse color. */
    const glm::vec3& getAmbientColor() const;       /**< Returns the ambient color. */
    const glm::vec3& getSpecularColor() const;      /**< Returns the specular color. */
    float getTransparencyFactor() const;            /**< Returns the transparency factor. */
    float getShininessExponent() const;             /**< Returns the shininess coefficient. */
    Texture* getAssociatedTexture();                /**< Returns the associated texture (if any). */

    // -------------------
    // Setters
    // -------------------
    void setSpecularColor(const glm::vec3& specularColor);     /**< Sets the specular color. */
    void setAmbientColor(const glm::vec3& ambientColor);       /**< Sets the ambient color. */
    void setTransparencyFactor(float transparencyFactor);      /**< Sets the transparency factor. */
    void setShininessExponent(float shininessExponent);        /**< Sets the shininess coefficient. */
    void setEmissionColor(const glm::vec3& emissionColor);     /**< Sets the emission color. */
    void setDiffuseColor(const glm::vec3& diffuseColor);       /**< Sets the diffuse color. */
    void setAssociatedTexture(Texture* associatedTexture);     /**< Sets the texture. */

    /**
     * @brief Applies the material to the current OpenGL context.
     *        Sets the colors, shininess, transparency, and binds the texture if present.
     */
    void render() override;

private:
    glm::vec3 diffuseColor;       /**< Diffuse color of the material. */
    glm::vec3 emissionColor;      /**< Emission color of the material. */
    glm::vec3 ambientColor;       /**< Ambient color of the material. */
    glm::vec3 specularColor;      /**< Specular color of the material. */
    float transparencyFactor;     /**< Transparency factor (0 = opaque, 1 = fully transparent). */
    float shininessExponent;      /**< Specular exponent controlling highlight size. */
    Texture* associatedTexture;   /**< Associated texture (if any). */
};