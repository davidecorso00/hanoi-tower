#pragma once

#include "Camera.h"

/**
 * @file TestCamera.h
 * @brief Declarations of unit tests for the Camera class.
 *
 * This file contains the declarations of the tests that validate the correct
 * behavior of the Camera class, in particular the management of the projection
 * matrix and the computation of the view (inverse) matrix.
 */

/**
 * @namespace CameraTest
 * @brief Namespace containing unit tests for the Camera class.
 *
 * All test functions related to the Camera class are grouped in this namespace
 * to keep test logic separated from production code.
 */
namespace CameraTest {

    /**
     * @brief Runs all unit tests for the Camera class.
     *
     * The tests perform, among others, the following checks:
     * - initialization of the default projection matrix;
     * - correct behavior of projection matrix getters and setters;
     * - correct computation of the view matrix (inverse of the camera matrix).
     *
     * If any check fails, an assertion aborts execution and highlights
     * the issue.
     */
    void test();
}