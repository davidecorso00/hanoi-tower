#pragma once

#include "Lib.h"
#include "Node.h"

#include <glm/glm.hpp>

/**
 * @brief Abstract base class representing a light source in the scene.
 *
 * This class derives from Node and encapsulates common light properties such as
 * ambient, diffuse, specular components, and world-space position.
 * Specific types of lights (directional, point, spot, etc.) should inherit and
 * extend this class.
 */
class ENG_API Light : public Node {
public:
    /**
     * @brief Default constructor.
     *
     * Initializes all light components to zero and assigns a default transform.
     */
    Light();

    /**
     * @brief Constructs a light with a name and an initial transformation matrix.
     * @param name Name assigned to the light.
     * @param matrix Initial transformation matrix applied to the light.
     */
    Light(const std::string& lightName, const glm::mat4& initialTransformationMatrix);

    /**
     * @brief Pure virtual destructor.
     *
     * Ensures that derived classes correctly clean up resources.
     */
    virtual ~Light() = 0;

    // -------------------
    // Getters
    // -------------------
    /**
     * @brief Returns the ambient component of the light.
     * @return A reference to a glm::vec4 representing the ambient color.
     */
    const glm::vec4& getAmbientLightComponent() const;

    /**
     * @brief Returns the diffuse component of the light.
     * @return A reference to a glm::vec4 representing the diffuse color.
     */
    const glm::vec4& getDiffuseLightComponent() const;

    /**
     * @brief Returns the specular component of the light.
     * @return A reference to a glm::vec4 representing the specular color.
     */
    const glm::vec4& getSpecularLightComponent() const;

    /**
     * @brief Returns the world-space position of the light.
     * @return A reference to a glm::vec4 representing the position.
     *
     * For directional lights, this may represent a direction instead.
     */
    const glm::vec4& getWorldSpaceLightPosition() const;

    // -------------------
    // Setters
    // -------------------
    /**
     * @brief Sets the ambient component of the light.
     * @param ambient New ambient component.
     */
    void setAmbientLightComponent(const glm::vec4& c);

    /**
     * @brief Sets the diffuse color of the light.
     * @param diffuse New diffuse component.
     */
    void setDiffuseLightComponent(const glm::vec4& c);

    /**
     * @brief Sets the specular color of the light.
     * @param specular New specular component.
     */
    void setSpecularLightComponent(const glm::vec4& c);

    /**
     * @brief Sets the world-space position of the light.
     * @param position New position.
     */
    void setWorldSpaceLightPosition(const glm::vec4& position);

    /**
     * @brief Assigns a unique ID to the light for use by the rendering system.
     * @param id Identifier used by the renderer to distinguish this light.
     */
    void setLightId(int id);

    /**
     * @brief Renders the light.
     *
     * Overrides the Node render method. Typically, lights do not render geometry,
     * but this function allows derived classes to perform debug rendering or
     * upload light data to shaders.
     */
    virtual void render() override;

protected:
    int lightId;                            ///< ID used by the renderer to reference this light.

    glm::vec4 worldSpaceLightPosition;      ///< World-space position of the light.
    glm::vec4 ambientLightComponent;        ///< Ambient light component.
    glm::vec4 diffuseLightComponent;        ///< Diffuse light component.
    glm::vec4 specularLightComponent;       ///< Specular light component.
};