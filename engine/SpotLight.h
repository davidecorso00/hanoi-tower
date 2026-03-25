#pragma once

#include "Light.h"
#include <glm/glm.hpp>

/**
 * @brief Represents a spotlight in the scene.
 *
 * This class derives from Light and adds spotlight-specific properties such as
 * direction, cutoff angle, and attenuation exponent.
 */
class ENG_API SpotLight : public Light {
public:
    /**
     * @brief Default constructor.
     *
     * Creates a spotlight with default parameters.
     */
    SpotLight();

    /**
     * @brief Fully-initializing constructor.
     *
     * @param name Name assigned to the spotlight.
     * @param matrix Initial transformation matrix.
     * @param ambient Ambient light Component.
     * @param diffuse Diffuse light Component.
     * @param specular Specular light Component.
     * @param direction Direction of the spotlight.
     * @param cutoff Cutoff angle of the spotlight (in degrees).
     * @param exponent Spotlight exponent used for angular attenuation.
     */
    SpotLight(
        const std::string& spotLightName,
        const glm::mat4& initialTransformationMatrix,
        const glm::vec3& ambientComponent,
        const glm::vec3& diffuseComponent,
        const glm::vec3& specularComponent,
        const glm::vec3& direction,
        float cutoffAngle,
        float exponent
    );

    /**
     * @brief Destructor.
     */
    ~SpotLight();

    // -------------------
    // Getters
    // -------------------
    /**
     * @brief Returns the cutoff angle of the spotlight.
     * @return Cutoff angle in degrees.
     */
    float getCutoffAngle() const;

    /**
     * @brief Sets the cutoff angle of the spotlight.
     * @param value New cutoff angle in degrees.
     */
    void setCutoffAngle(float cutoffAngleToSet);

    /**
     * @brief Imposta la direzione della luce spot.
     * @param dir nuova direzione.
     */
    void setDirection(const glm::vec3& directionToSet);

    /**
     * @brief Restituisce la direzione della luce spot.
     * @return direzione come glm::vec3.
     */
    glm::vec3 getDirection() const;

    /**
     * @brief Sets the spotlight exponent.
     *
     * The exponent controls how the light intensity falls off
     * from the center of the spotlight cone.
     *
     * @param exp New spotlight exponent value.
     */
    void setExponent(float exponentToSet);

    /**
     * @brief Returns the spotlight exponent.
     * @return Spotlight exponent value.
     */
    float getExponent() const;

    /**
     * @brief Renders the spotlight using the OpenGL fixed pipeline.
     *
     * Uploads parameters such as direction, cutoff angle and exponent to OpenGL.
     */
    void render() override;

private:
    glm::vec3 direction = glm::vec3(0.0f, 0.0f, -1.0f);

    float cutoffAngle = 30.0f;
    float exponent = 0.0f;
};