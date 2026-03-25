#include "DirectionalLight.h"

#include <GL/freeglut.h>
#include <glm/gtc/type_ptr.hpp>

// -------------------
// Default constructor
// -------------------
DirectionalLight::DirectionalLight() : Light() {
    // Slide 40/53: infite light settings
    // - w is zero
    glm::vec4 pos = glm::vec4(0.0f, 0.0f, -1.0f, 0.0f);
    setWorldSpaceLightPosition(pos);
}

// ------------------------------
// Fully-initializing constructor
// ------------------------------
DirectionalLight::DirectionalLight(
    const std::string& directionalLightName,
    const glm::mat4& initialTransformationMatrix,
    const glm::vec3& ambientComponent,
    const glm::vec3& diffuseComponent,
    const glm::vec3& specularComponent,
    const glm::vec3& direction)
    : Light(directionalLightName, initialTransformationMatrix)
{
    setAmbientLightComponent(glm::vec4(ambientComponent, 1.0f));
    setDiffuseLightComponent(glm::vec4(diffuseComponent, 1.0f));
    setSpecularLightComponent(glm::vec4(specularComponent, 1.0f));

    setDirection(direction);
}

// ----------
// Destructor
// ----------
DirectionalLight::~DirectionalLight() {}

// -----------------
// Getters & Setters
// -----------------
void DirectionalLight::setDirection(const glm::vec3& directionToSet) {
    setWorldSpaceLightPosition(glm::vec4(glm::normalize(directionToSet), 0.0f));
}

glm::vec3 DirectionalLight::getDirection() const {
    const glm::vec4 directionalVector = getWorldSpaceLightPosition();
    return glm::vec3(directionalVector);
}

// ---------
// Rendering
// ---------
void DirectionalLight::render() {
    if (lightId >= 0) {
        glEnable(lightId);
    }
    else {
        return;
    }

    // Light properties - color
    glLightfv(lightId, GL_AMBIENT, glm::value_ptr(getAmbientLightComponent()));
    glLightfv(lightId, GL_DIFFUSE, glm::value_ptr(getDiffuseLightComponent()));
    glLightfv(lightId, GL_SPECULAR, glm::value_ptr(getSpecularLightComponent()));

    // Slide 40/52: w component is zero
    glLightfv(lightId, GL_POSITION, glm::value_ptr(getWorldSpaceLightPosition()));
}