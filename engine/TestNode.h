#pragma once

#include "Node.h"

/**
 * @file TestNode.h
 * @brief Declarations of unit tests for the Node class.
 *
 * The tests defined in this file verify the correct behavior of the
 * Node class within the scene graph (parent/children handling,
 * local transformations and world matrix computation).
 */

/**
 * @namespace NodeTest
 * @brief Namespace containing unit tests for the Node class.
 */
namespace NodeTest {

    /**
     * @brief Runs all unit tests for the Node class.
     *
     * It checks:
     * - handling of the parent node;
     * - adding and removing child nodes;
     * - recursive search by name;
     * - application of local transformations and world matrix computation.
     */
    void test();
}
