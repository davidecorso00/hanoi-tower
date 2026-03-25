#include "PerspectiveCamera.h"

/**
 * @file PerspectiveCamera.cpp
 * @brief Implementation of the PerspectiveCamera class.
 */

PerspectiveCamera::PerspectiveCamera(
    const std::string& name,
    float fovY,
    float aspect,
    float near,
    float far
)
    : Camera(name),
    fovY(fovY),
    aspect(aspect),
    zNear(near),
    zFar(far)
{
    // Initialize the projection matrix using the provided parameters
    updateProjection();
}

void PerspectiveCamera::setFov(float fov) {
    // Update the vertical field of view (in degrees)
    this->fovY = fov;

    // Recompute the projection matrix
    updateProjection();
}

void PerspectiveCamera::setAspectRatio(float width, float height) {
    // Prevent division by zero when the window is minimized
    if (height <= 0.0f)
        height = 1.0f;

    // Update the aspect ratio (width / height)
    this->aspect = width / height;

    // Recompute the projection matrix
    updateProjection();
}

void PerspectiveCamera::setNear(float near) {
    // Update the near clipping plane distance
    this->zNear = near;

    // Recompute the projection matrix
    updateProjection();
}

void PerspectiveCamera::setFar(float far) {
    // Update the far clipping plane distance
    this->zFar = far;

    // Recompute the projection matrix
    updateProjection();
}

void PerspectiveCamera::updateProjection() {
    // glm::perspective expects the field of view in radians
    this->cameraProjectionMatrix = glm::perspective(glm::radians(fovY), aspect, zNear, zFar);
}
