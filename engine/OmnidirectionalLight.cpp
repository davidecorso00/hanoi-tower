#include "OmnidirectionalLight.h"

#include <GL/freeglut.h>
#include <glm/gtc/type_ptr.hpp>

// -------------------
// Default constructor
// -------------------
OmnidirectionalLight::OmnidirectionalLight() : Light()
{
}

// ------------------------------
// Fully-initializing constructor
// ------------------------------
OmnidirectionalLight::OmnidirectionalLight(
    const std::string& omniLightName,
    const glm::mat4& initialTransformationMatrix,
    const glm::vec3& ambientComponent,
    const glm::vec3& diffuseComponent,
    const glm::vec3& specularComponent
)
    : Light(omniLightName, initialTransformationMatrix)
{
    // Slide 41/53 -> w is one
    setWorldSpaceLightPosition({ glm::vec3(initialTransformationMatrix[3]), 1.0f });

    setAmbientLightComponent(glm::vec4(ambientComponent, 1.0f));
    setDiffuseLightComponent(glm::vec4(diffuseComponent, 1.0f));
    setSpecularLightComponent(glm::vec4(specularComponent, 1.0f));
}

// ----------
// Destructor
// ----------
OmnidirectionalLight::~OmnidirectionalLight() {}

// ------
// Getter
// ------
float OmnidirectionalLight::getCutoffAngle() const {
    return specialCutoff;
}

// ---------
// Rendering
// ---------
void OmnidirectionalLight::render() {
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

    // Omni settings
    GLfloat localPosition[4] = { 0.0f, 0.0f, 0.0f, 1.0f };
    
    glLightfv(lightId, GL_POSITION, (localPosition));
    glLightf(lightId, GL_SPOT_CUTOFF, specialCutoff);

    // Light properties - attenuation (specific to omni and spot) <- review slide 42/53
    glLightf(lightId, GL_CONSTANT_ATTENUATION, 1.0f);   // default 1
    glLightf(lightId, GL_LINEAR_ATTENUATION, 0.0f);     // default 0
    glLightf(lightId, GL_QUADRATIC_ATTENUATION, 0.0f);  // default 0
}