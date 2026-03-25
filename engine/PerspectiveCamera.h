#pragma once

#include "Camera.h"

/**
 * @class PerspectiveCamera
 * @brief Camera implementing a perspective projection.
 *
 * This camera simulates human or photographic vision, where objects
 * that are farther from the camera appear smaller.
 * It is the most commonly used projection for 3D scenes and games.
 */
class ENG_API PerspectiveCamera : public Camera {
public:
    /**
     * @brief Constructs a perspective camera.
     *
     * Initializes a perspective projection using the specified parameters.
     *
     * @param name   Camera name.
     * @param fovY   Vertical field of view angle (in degrees).
     * @param aspect Aspect ratio of the viewport (width / height).
     * @param near   Distance to the near clipping plane.
     * @param far    Distance to the far clipping plane.
     */
    PerspectiveCamera(
        const std::string& name,
        float fovY,
        float aspect,
        float near,
        float far
    );

    /**
     * @brief Sets the vertical field of view.
     *
     * Updates the vertical field of view (FOV) and recalculates
     * the projection matrix.
     *
     * @param fovY New vertical FOV angle in degrees.
     */
    void setFov(float fovY);

    /**
     * @brief Updates the aspect ratio of the camera.
     *
     * The aspect ratio is computed as width divided by height.
     * This method should be called when the viewport size changes.
     *
     * @param width  Viewport width.
     * @param height Viewport height.
     */
    void setAspectRatio(float width, float height);

    /**
     * @brief Sets the near clipping plane distance.
     *
     * @param near New near plane distance.
     */
    void setNear(float near);

    /**
     * @brief Sets the far clipping plane distance.
     *
     * @param far New far plane distance.
     */
    void setFar(float far);

    /**
     * @brief Recomputes the perspective projection matrix.
     *
     * This method updates the projection matrix using the current
     * camera parameters. It is automatically called by the setters.
     */
    void updateProjection();

private:
    /** Vertical field of view (in degrees). */
    float fovY;

    /** Viewport aspect ratio (width / height). */
    float aspect;

    /** Near clipping plane distance. */
    float zNear;

    /** Far clipping plane distance. */
    float zFar;
};