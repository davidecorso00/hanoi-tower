#include "TestHanoi.h"

#include <iostream>

/**
 * @file TestClient.cpp
 * @brief Test suite entry point for the client-side logic (Hanoi game).
 *
 * This file defines a simple function that aggregates all the unit
 * tests related to the client module, currently focused on the
 * Hanoi game logic.
 */

namespace ClientTestSuite {

    /**
     * @brief Runs all client-side unit tests.
     *
     * At the moment this just calls HanoiTest::test(), but it can be
     * easily extended if more client-side test namespaces are added.
     */
    void runAll() {
        std::cout << "===== Running CLIENT test suite =====" << std::endl;

        HanoiTest::test();

        std::cout << "===== CLIENT test suite completed successfully =====" << std::endl;
    }
}

int main() {
    ClientTestSuite::runAll();
    return 0;
}

