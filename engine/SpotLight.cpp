#include "SpotLight.h"

#include <GL/freeglut.h>
#include <glm/gtc/type_ptr.hpp>

// -------------------
// Default constructor
// -------------------
SpotLight::SpotLight()
    : Light(),
    exponent(0.0f)
{
    direction = glm::vec3(0.0f, 0.0f, -1.0f);
}

// ------------------------------
// Fully-initializing constructor
// ------------------------------
SpotLight::SpotLight(
    const std::string& spotLightName,
    const glm::mat4& initialTransformationMatrix,
    const glm::vec3& ambientComponent,
    const glm::vec3& diffuseComponent,
    const glm::vec3& specularComponent,
    const glm::vec3& direction,
    float cutoffAngle,
    float exponent)
    : Light(spotLightName, initialTransformationMatrix),
    cutoffAngle(cutoffAngle),
    exponent(exponent)
{
    setDirection(direction);

    setSpecularLightComponent(glm::vec4(specularComponent, 1.0f));
    setAmbientLightComponent(glm::vec4(ambientComponent, 1.0f));
    setDiffuseLightComponent(glm::vec4(diffuseComponent, 1.0f));
}

// ----------
// Destructor
// ----------
SpotLight::~SpotLight() {}

// -----------------
// Getters & Setters
// -----------------
float SpotLight::getCutoffAngle() const { return cutoffAngle; }
glm::vec3 SpotLight::getDirection() const { return direction; }
float SpotLight::getExponent() const { return exponent; }

void SpotLight::setCutoffAngle(float cutoffAngleToSet) { cutoffAngle = cutoffAngleToSet; }
void SpotLight::setDirection(const glm::vec3& directionToSet) { direction = glm::normalize(directionToSet); }
void SpotLight::setExponent(float exponentToSet) { exponent = exponentToSet; }

// ---------
// Rendering
// ---------
void SpotLight::render() {
    if (lightId >= 0) {
        glEnable(lightId);
    }
    else {
        return;
    }

    // Light properties - color components
    glLightfv(lightId, GL_AMBIENT, glm::value_ptr(getAmbientLightComponent()));
    glLightfv(lightId, GL_DIFFUSE, glm::value_ptr(getDiffuseLightComponent()));
    glLightfv(lightId, GL_SPECULAR, glm::value_ptr(getSpecularLightComponent()));

    // SpotLight settings
    GLfloat localPosition[4] = { 0.0f, 0.0f, 0.0f, 1.0f };
    GLfloat localDirection[3] = { 0.0f, -1.0f, 0.0f };

    glLightfv(lightId, GL_POSITION, (localPosition));
    glLightfv(lightId, GL_SPOT_DIRECTION, (localDirection));
    glLightf(lightId, GL_SPOT_CUTOFF, cutoffAngle);    // Cone parameters
    glLightf(lightId, GL_SPOT_EXPONENT, exponent);     // Cone parameters

    // Light properties - attenuation (specific to omni and spot) <- review slide 42/53
    glLightf(lightId, GL_CONSTANT_ATTENUATION, 1.0f);  // default 1
    glLightf(lightId, GL_LINEAR_ATTENUATION, 0.0f);    // default 0
    glLightf(lightId, GL_QUADRATIC_ATTENUATION, 0.0f); // default 0
}