#pragma once

#include "Object.h"

/**
 * @file TestObject.h
 * @brief Declarations of unit tests for the Object class.
 *
 * This file defines the test function that verifies the basic
 * behavior of the Object class, such as the name and the handling
 * of the unique identifier.
 */

/**
 * @namespace ObjectTest
 * @brief Namespace containing unit tests for the Object class.
 */
namespace ObjectTest {

    /**
     * @brief Runs all unit tests for the Object class.
     *
     * It verifies in particular:
     * - assignment and retrieval of the object name;
     * - generation of unique and sequential identifiers for multiple instances.
     */
    void test();
}