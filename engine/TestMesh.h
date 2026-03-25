#pragma once

#include "Mesh.h"

/**
 * @file TestMesh.h
 * @brief Declarations of unit tests for the Mesh class.
 *
 * This file contains the declaration of the test function that verifies
 * the correct behavior of the Mesh class, in particular the handling
 * of vertices, normals, texture coordinates and faces.
 */

/**
 * @namespace MeshTest
 * @brief Namespace containing unit tests for the Mesh class.
 */
namespace MeshTest {

    /**
     * @brief Runs the basic unit tests for the Mesh class.
     *
     * It verifies the getter/setter methods for:
     * - vertices;
     * - normals;
     * - texture coordinates;
     * - faces.
     */
    void test();
}