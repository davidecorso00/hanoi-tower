#pragma once

#include "Node.h"
#include "Lib.h"

#include <glm/glm.hpp>

/**
 * @class Camera
 * @brief Camera node used to view the 3D scene.
 *
 * The Camera class represents a viewing camera within the scene graph.
 * It inherits from Node, allowing it to be positioned and transformed
 * hierarchically. The camera manages a projection matrix and provides
 * utilities to compute the inverse view matrix used for rendering.
 */
class ENG_API Camera : public Node {
public:
    /**
     * @brief Constructs a Camera node with the given name.
     *
     * The camera is initialized with an identity transformation and
     * a projection matrix that can be set later.
     *
     * @param name Name of the camera node.
     */
    Camera(const std::string& cameraName);

    /**
     * @brief Destructor.
     */
    ~Camera();

    // -------------------
    // Projection handling
    // -------------------

    // Getters

    /**
     * @brief Returns the camera projection matrix.
     *
     * The projection matrix defines how 3D coordinates are projected
     * into the 2D viewport (e.g. perspective or orthographic projection).
     *
     * @return Constant reference to the projection matrix.
     */
    const glm::mat4& getCameraProjectionMatrix() const;

    /**
     * @brief Computes the inverse camera transformation matrix.
     *
     * This matrix converts world-space coordinates into camera-space
     * coordinates and is typically used as the View matrix in
     * the rendering pipeline.
     *
     * @return The inverse camera (view) transformation matrix.
     */
    glm::mat4 getInverseCameraMatrix() const;

    // Setter

    /**
     * @brief Sets the camera projection matrix.
     *
     * @param projectionMatrix The new projection matrix to be used.
     */
    void setCameraProjectionMatrix(const glm::mat4& cameraProjectionMatrix);

protected:
    /** @brief Camera projection matrix. */
    glm::mat4 cameraProjectionMatrix;
};