#include "OrthographicCamera.h"

/**
 * @file OrthographicCamera.cpp
 * @brief Implementation of the OrthographicCamera class.
 */

OrthographicCamera::OrthographicCamera(
    const std::string& name,
    float left,
    float right,
    float bottom,
    float top,
    float near,
    float far
)
    : Camera(name),
    left(left),
    right(right),
    bottom(bottom),
    top(top),
    zNear(near),
    zFar(far)
{
    // Initialize the orthographic projection matrix
    updateProjection();
}

void OrthographicCamera::setFrustum(
    float l,
    float r,
    float b,
    float t,
    float n,
    float f
) {
    // Update orthographic frustum boundaries
    left = l;
    right = r;
    bottom = b;
    top = t;
    zNear = n;
    zFar = f;

    // Recompute the projection matrix
    updateProjection();
}

void OrthographicCamera::updateProjection() {
    // Compute the orthographic projection matrix using current frustum parameters
    this->cameraProjectionMatrix = glm::ortho(left, right, bottom, top, zNear, zFar);
}