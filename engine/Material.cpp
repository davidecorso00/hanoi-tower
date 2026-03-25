#include "Material.h"

#include <GL/freeglut.h>
#include <glm/gtc/type_ptr.hpp>

// ------------------------------------------------------------------
// Constructors
// ------------------------------------------------------------------
Material::Material(
    const std::string& materialName,
    const glm::vec3& emissionColor,
    const glm::vec3& ambientColor,
    const glm::vec3& diffuseColor,
    const glm::vec3& specularColor,
    float shininessExponent,
    float transparencyFactor)
    : Object(materialName),
    emissionColor(emissionColor),
    ambientColor(ambientColor),
    diffuseColor(diffuseColor),
    specularColor(specularColor),
    shininessExponent(shininessExponent),
    transparencyFactor(transparencyFactor),
    associatedTexture(nullptr)
{
}

// ------------------------------------------------------------------
// Destructor
// ------------------------------------------------------------------
Material::~Material() {}


// ------------------------------------------------------------------
// Getters & Setters
// ------------------------------------------------------------------
const glm::vec3& Material::getEmissionColor() const {
    return emissionColor;
}

void Material::setEmissionColor(const glm::vec3& color) {
    emissionColor = color;
}




const glm::vec3& Material::getDiffuseColor() const {
    return diffuseColor;
}

void Material::setDiffuseColor(const glm::vec3& color) {
    diffuseColor = color;
}




const glm::vec3& Material::getAmbientColor() const {
    return ambientColor;
}

void Material::setAmbientColor(const glm::vec3& color) {
    ambientColor = color;
}




const glm::vec3& Material::getSpecularColor() const {
    return specularColor;
}

void Material::setSpecularColor(const glm::vec3& color) {
    specularColor = color;
}




float Material::getTransparencyFactor() const {
    return transparencyFactor;
}

void Material::setTransparencyFactor(float factor) {
    transparencyFactor = factor;
}




float Material::getShininessExponent() const {
    return shininessExponent;
}

void Material::setShininessExponent(float exp) {
    shininessExponent = exp;
}




Texture* Material::getAssociatedTexture() {
    return associatedTexture;
}

void Material::setAssociatedTexture(Texture* texturePtr) {
    associatedTexture = texturePtr;
}


// ------------------------------------------------------------------
// Render
// ------------------------------------------------------------------
void Material::render() {
    // Set Phong/Blinn-Phong shading parameters
    glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, glm::value_ptr(glm::vec4(ambientColor, transparencyFactor)));
    glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, glm::value_ptr(glm::vec4(diffuseColor, transparencyFactor)));
    glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, glm::value_ptr(glm::vec4(specularColor, transparencyFactor)));
    glMaterialfv(GL_FRONT_AND_BACK, GL_EMISSION, glm::value_ptr(glm::vec4(emissionColor, transparencyFactor)));
    glMaterialf(GL_FRONT_AND_BACK, GL_SHININESS, shininessExponent);

    if (associatedTexture) {
        glEnable(GL_TEXTURE_2D);
        glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
        associatedTexture->render();
    }
    else {
        glDisable(GL_TEXTURE_2D);
    }
}