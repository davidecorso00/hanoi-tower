#pragma once

#include "Engine.h"

/**
 * @file TestTexture.h
 * @brief Declarations of unit tests for the Texture class.
 *
 * This file contains the declaration of the test function that verifies
 * the correct behavior of the Texture class, from creation to the
 * render/bind phase.
 */

/**
 * @namespace TextureTest
 * @brief Namespace containing unit tests for the Texture class.
 */
namespace TextureTest {

    /**
     * @brief Runs all unit tests for the Texture class.
     *
     * The tests cover:
     * - creation of textures with valid name and path;
     * - render/bind calls without crashes;
     * - creation of multiple independent instances.
     */
    void test();
}
