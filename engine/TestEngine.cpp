#include "TestCamera.h"
#include "TestLight.h"
#include "TestMaterial.h"
#include "TestMesh.h"
#include "TestNode.h"
#include "TestObject.h"
#include "TestTexture.h"

#include <iostream>

/**
 * @file TestEngine.cpp
 * @brief Test suite entry point for the whole rendering engine.
 *
 * This file defines a single function that runs all unit tests
 * defined in the various Test*.cpp files of the engine module.
 */

namespace EngineTestSuite {

    /**
     * @brief Runs all engine-related unit tests.
     *
     * This function simply calls, in sequence, all the individual
     * test entry points defined in the engine test namespaces.
     * Any failing assertion inside those functions will stop
     * the execution and highlight the issue.
     */
    void runAll() {
        std::cout << "===== Running ENGINE test suite =====" << std::endl;

        CameraTest::test();

        SpotLightTest::test();
        DirectionalLightTest::test();
        OmnidirectionalLightTest::test();

        MaterialTest::test();
        MeshTest::test();
        NodeTest::test();
        ObjectTest::test();

        std::cout << "===== ENGINE test suite completed successfully =====" << std::endl;
    }
}

int main() {
    EngineTestSuite::runAll();
    return 0;
}

