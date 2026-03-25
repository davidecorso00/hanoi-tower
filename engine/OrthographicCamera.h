#pragma once

#include "Camera.h"

/**
 * @class OrthographicCamera
 * @brief Camera implementing an orthographic projection.
 *
 * This class extends the Camera class by providing an orthographic projection.
 * It is typically used for:
 * - 2D rendering
 * - minimaps
 * - UI overlays
 * - technical and CAD-style views
 *
 * Unlike perspective projection, object sizes do not change with distance
 * from the camera.
 */
class ENG_API OrthographicCamera : public Camera {
public:

    /**
     * @brief Constructs an orthographic camera.
     *
     * Initializes the orthographic frustum using the specified boundaries.
     *
     * @param name   Camera name.
     * @param left   Left boundary of the view volume.
     * @param right  Right boundary of the view volume.
     * @param bottom Bottom boundary of the view volume.
     * @param top    Top boundary of the view volume.
     * @param near   Near clipping plane distance.
     * @param far    Far clipping plane distance.
     */
    OrthographicCamera(
        const std::string& name,
        float left, float right,
        float bottom, float top,
        float near, float far
    );

    /**
     * @brief Updates the orthographic frustum parameters.
     *
     * Sets new boundaries for the orthographic view volume and
     * updates the projection matrix accordingly.
     *
     * @param left   Left boundary of the view volume.
     * @param right  Right boundary of the view volume.
     * @param bottom Bottom boundary of the view volume.
     * @param top    Top boundary of the view volume.
     * @param near   Near clipping plane distance.
     * @param far    Far clipping plane distance.
     */
    void setFrustum(
        float left,
        float right,
        float bottom,
        float top,
        float near,
        float far
    );

    /**
     * @brief Recomputes the orthographic projection matrix.
     *
     * This method recalculates the projection matrix using the
     * current frustum parameters.
     */
    void updateProjection();

private:

    /** Left boundary of the orthographic frustum. */
    float left;

    /** Right boundary of the orthographic frustum. */
    float right;

    /** Bottom boundary of the orthographic frustum. */
    float bottom;

    /** Top boundary of the orthographic frustum. */
    float top;

    /** Near clipping plane distance. */
    float zNear;

    /** Far clipping plane distance. */
    float zFar;
};