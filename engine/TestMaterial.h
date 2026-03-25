#pragma once

#include "Material.h"

/**
 * @file TestMaterial.h
 * @brief Declarations of unit tests for the Material class.
 *
 * This file declares the test function that validates the correct
 * behavior of the Material class, including colors, shininess
 * exponent, transparency factor and texture association.
 */

/**
 * @namespace MaterialTest
 * @brief Namespace containing unit tests for the Material class.
 */
namespace MaterialTest {

    /**
     * @brief Runs all unit tests for the Material class.
     *
     * It verifies in particular:
     * - getters and setters for colors (emission, ambient, diffuse, specular);
     * - handling of the shininess exponent;
     * - handling of the transparency factor;
     * - initial state of any associated texture.
     */
    void test();
}