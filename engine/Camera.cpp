#include "Camera.h"

#include <glm/gtc/matrix_transform.hpp>

/**
 * @brief Constructs a Camera node with identity projection.
 *
 * @param name Name of the camera node.
 */
Camera::Camera(const std::string& cameraName)
    : Node(cameraName),
    cameraProjectionMatrix(glm::mat4(1.0f))
{
}

/**
 * @brief Destructor.
 */
Camera::~Camera() {};

// Setter

/**
 * @brief Returns the inverse camera matrix (View matrix).
 *
 * Converts world coordinates into camera space.
 */
glm::mat4 Camera::getInverseCameraMatrix() const {
    return glm::inverse(this->getWorldMatrix());
}

// Getter

/**
 * @brief Sets the camera projection matrix.
 *
 * @param projectionMatrix New projection matrix.
 */
void Camera::setCameraProjectionMatrix(const glm::mat4& cameraProjectionMatrix) {
    this->cameraProjectionMatrix = cameraProjectionMatrix;
}

/**
 * @brief Returns the camera projection matrix.
 */
const glm::mat4& Camera::getCameraProjectionMatrix() const {
    return cameraProjectionMatrix;
}