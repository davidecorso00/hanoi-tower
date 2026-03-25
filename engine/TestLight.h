#pragma once

#include "Engine.h"
#include "SpotLight.h"
#include "DirectionalLight.h"
#include "OmnidirectionalLight.h"

/**
 * @file TestLight.h
 * @brief Declarations of unit tests for the different light types.
 *
 * This file contains the declarations of test functions for
 * SpotLight, DirectionalLight and OmnidirectionalLight, validating
 * the correct behavior of their main properties (direction, cutoff,
 * light components, etc.).
 */

/**
 * @namespace SpotLightTest
 * @brief Namespace containing unit tests for the SpotLight class.
 */
namespace SpotLightTest {

    /**
     * @brief Runs all unit tests for the SpotLight class.
     *
     * It mainly verifies the correct handling of direction,
     * cutoff angle and spotlight exponent.
     */
    void test();
}

/**
 * @namespace DirectionalLightTest
 * @brief Namespace containing unit tests for the DirectionalLight class.
 */
namespace DirectionalLightTest {

    /**
     * @brief Runs all unit tests for the DirectionalLight class.
     *
     * It verifies that the direction is correctly set and can be modified.
     */
    void test();
}

/**
 * @namespace OmnidirectionalLightTest
 * @brief Namespace containing unit tests for the OmnidirectionalLight class.
 */
namespace OmnidirectionalLightTest {

    /**
     * @brief Runs all unit tests for the OmnidirectionalLight class.
     *
     * Among other things, it verifies the default values of the cutoff angle.
     */
    void test();
}