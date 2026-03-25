#pragma once

#include "Light.h"

#include <glm/glm.hpp>

/**
 * @brief Represents an directional (infinite) light source.
 *
 * A directional light has no fixed position and simulates a light source
 * at an infinite distance, such as the sun. Only the light direction
 * affects the illumination.
 */
class ENG_API DirectionalLight : public Light {
public:
    /**
     * @brief Default constructor.
     *
     * Creates a directional light with a default direction pointing along the negative Z axis.
     */
    DirectionalLight();

    /**
     * @brief Fully-initializing constructor.
     *
     * @param name Name assigned to the node.
     * @param matrix Initial transformation matrix.
     * @param ambient Ambient light component.
     * @param diffuse Diffuse light component.
     * @param specular Specular light component.
     * @param direction Direction of the light.
     */
    DirectionalLight(
        const std::string& directionalLightName,
        const glm::mat4& initialTransformationMatrix,
        const glm::vec3& ambientComponent,
        const glm::vec3& diffuseComponent,
        const glm::vec3& specularComponent,
        const glm::vec3& direction
    );

    /**
     * @brief Destructor.
     */
    ~DirectionalLight();

    /**
     * @brief Sets the direction of the light.
     *
     * The direction vector is normalized before being stored.
     *
     * @param dir New light direction.
     */
    void setDirection(const glm::vec3& directionToSet);

    /**
     * @brief Returns the direction of the light.
     * @return Normalized light direction as a glm::vec3.
     */
    glm::vec3 getDirection() const;

    /**
     * @brief Renders the directional light using the OpenGL fixed pipeline.
     *
     * Uploads the light direction and color components to OpenGL.
     */
    void render() override;
};