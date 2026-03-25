#include "Light.h"

#include <GL/freeglut.h>
#include <glm/gtc/type_ptr.hpp>

// -------------------
// Default constructor
// -------------------
Light::Light()
    : Node("Light"),
    lightId(-1),
    worldSpaceLightPosition(0.0f, 0.0f, 0.0f, 1.0f),
    ambientLightComponent(0.2f),
    diffuseLightComponent(1.0f),
    specularLightComponent(1.0f)
{
}

// -----------------------------------------------
// Constructor with name and transformation matrix
// -----------------------------------------------
Light::Light(const std::string& lightName, const glm::mat4& initialTransformationMatrix)
    : Node(lightName),
    lightId(-1),
    worldSpaceLightPosition(0.0f, 0.0f, 0.0f, 1.0f),
    ambientLightComponent(0.2f),
    diffuseLightComponent(1.0f),
    specularLightComponent(1.0f)
{
    this->setLocalMatrix(initialTransformationMatrix);
}


// -----------------------
// Pure virtual destructor
// -----------------------
Light::~Light() {}


// -----------------
// Getters & Setters
// -----------------
const glm::vec4& Light::getWorldSpaceLightPosition() const { return worldSpaceLightPosition; }
const glm::vec4& Light::getAmbientLightComponent() const { return ambientLightComponent; }
const glm::vec4& Light::getDiffuseLightComponent() const { return diffuseLightComponent; }
const glm::vec4& Light::getSpecularLightComponent() const { return specularLightComponent; }

void Light::setWorldSpaceLightPosition(const glm::vec4& position) { worldSpaceLightPosition = position; }
void Light::setAmbientLightComponent(const glm::vec4& c) { ambientLightComponent = c; }
void Light::setDiffuseLightComponent(const glm::vec4& c) { diffuseLightComponent = c; }
void Light::setSpecularLightComponent(const glm::vec4& c) { specularLightComponent = c; }
void Light::setLightId(int id) { lightId = id; }

// ---------
// Rendering
// ---------

/**
* STEPS
* 1. Check on the id to see if the light is active
* 2. If active, upload of all components and position
*/
void Light::render() {
    if (lightId < 0) return;
    glEnable(lightId);

    // Upload the ambient light component to OpenGL
    glLightfv(lightId, GL_AMBIENT, glm::value_ptr(ambientLightComponent));
    
    // Upload the diffuse light component
    glLightfv(lightId, GL_DIFFUSE, glm::value_ptr(diffuseLightComponent));
    
    // Upload the specular light component
    glLightfv(lightId, GL_SPECULAR, glm::value_ptr(specularLightComponent));
    
    // Upload the light's position
    glLightfv(lightId, GL_POSITION, glm::value_ptr(worldSpaceLightPosition));
}