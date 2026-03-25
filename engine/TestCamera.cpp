#include "TestCamera.h"
#include <iostream>
#include <cassert>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

/**
 * @brief Runs all unit tests for the Camera class.
 *
 * This function tests the correct initialization and behavior of the
 * Camera class, including projection matrix management and inverse
 * camera matrix computation.
 */
namespace CameraTest {

void test() {
    std::cout << "Running Camera tests..." << std::endl;

    // --------------------------------------------------
    // Camera creation
    // --------------------------------------------------
    Camera camera("TestCamera");

    // --------------------------------------------------
    // Test 1: Default projection matrix
    // --------------------------------------------------
    glm::mat4 proj = camera.getCameraProjectionMatrix();
    glm::mat4 identity(1.0f);
    assert(proj == identity);
    std::cout << "\tDefault projection matrix OK." << std::endl;

    // --------------------------------------------------
    // Test 2: Projection matrix setter and getter
    // --------------------------------------------------
    glm::mat4 newProj = glm::perspective(
        glm::radians(45.0f),
        4.0f / 3.0f,
        0.1f,
        100.0f
    );

    camera.setCameraProjectionMatrix(newProj);
    assert(camera.getCameraProjectionMatrix() == newProj);
    std::cout << "\tProjection matrix setter/getter OK." << std::endl;

    // --------------------------------------------------
    // Test 3: Inverse camera matrix computation
    // --------------------------------------------------
    glm::mat4 trans = glm::translate(
        glm::mat4(1.0f),
        glm::vec3(1.0f, 2.0f, 3.0f)
    );

    camera.setLocalMatrix(trans);

    glm::mat4 inv = camera.getInverseCameraMatrix();
    glm::mat4 expected = glm::inverse(trans);

    assert(inv == expected);
    std::cout << "\tInverse camera matrix OK." << std::endl;

    std::cout << "Camera tests passed!" << std::endl;
}

} // namespace CameraTest