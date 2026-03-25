#pragma once

#include "Hanoi.h"

/**
 * @file TestHanoi.h
 * @brief Declarations of unit tests for the Hanoi game logic class.
 *
 * This file contains the declaration of the test function that verifies the
 * core behavior of the Hanoi class (initialization, input handling, camera
 * helpers and undo/redo system).
 */

/**
 * @namespace HanoiTest
 * @brief Namespace containing unit tests for the Hanoi class.
 */
namespace HanoiTest {

    /**
     * @brief Runs all unit tests for the Hanoi class.
     *
     * The tests cover, among others:
     * - correct initialization of the game state from a minimal scene graph;
     * - basic interaction through specialCallback (pick up disc);
     * - camera helpers (presets and boundary clamping);
     * - undo/redo stack behavior after a simple action.
     */
    void test();
}


