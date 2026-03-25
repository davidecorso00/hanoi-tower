#pragma once

#include "Light.h"

#include <glm/glm.hpp>

/**
 * @brief Represents an omnidirectional (point) light source.
 *
 * An omnidirectional light emits light uniformly in all directions
 * from a single point in space. This corresponds to a standard
 * OpenGL point light.
 */
class ENG_API OmnidirectionalLight : public Light {
public:
    /**
     * @brief Default constructor.
     *
     * Creates a point light with default parameters.
     */
    OmnidirectionalLight();

    /**
     * @brief Fully-initializing constructor.
     *
     * @param name Name assigned to the node.
     * @param matrix Initial transformation matrix.
     * @param ambient Ambient light component.
     * @param diffuse Diffuse light component.
     * @param specular Specular light component.
     */
    OmnidirectionalLight(
        const std::string& omniLightName,
        const glm::mat4& initialTransformationMatrix,
        const glm::vec3& ambientComponent,
        const glm::vec3& diffuseComponent,
        const glm::vec3& specularComponent
    );

    /**
     * @brief Destructor.
     */
    ~OmnidirectionalLight();

    /**
     * @brief Returns the cutoff angle of the light.
     *
     * For an omnidirectional light, the cutoff angle is always 180 degrees,
     * meaning the light emits uniformly in all directions.
     *
     * @return Cutoff angle in degrees (always 180.0f).
     */
    float getCutoffAngle() const;

    /**
     * @brief Renders the omnidirectional light using the OpenGL fixed pipeline.
     */
    void render() override;

private:
    // Slide 41/53 -> special cutoff value to get a omni light
    float specialCutoff = 180.0f; ///< Special cutoff value
};
