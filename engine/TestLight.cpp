#include <iostream>
#include <cassert>
#include <glm/glm.hpp>
#include "TestLight.h"

void SpotLightTest::test() {
    std::cout << "Running SpotLight tests..." << std::endl;

    glm::mat4 model_view = glm::mat4(1.0f);
    glm::vec3 ambient(0.1f);
    glm::vec3 diffuse(1.0f);
    glm::vec3 specular(1.0f);
    glm::vec3 direction(1.0f, 0.0f, 0.0f);
    float cutoff = 15.0f;
    float exponent = 2.0f;

    SpotLight light("SpotLightTest", model_view, ambient, diffuse, specular, direction, cutoff, exponent);

    // Test direzione
    std::cout << "\tTesting Direction..." << std::endl;
    assert(light.getDirection() == direction);
    glm::vec3 newDirection(0.0f, 1.0f, 0.0f);
    light.setDirection(newDirection);
    assert(light.getDirection() == newDirection);
    std::cout << "\tDirection test OK." << std::endl;

    // Test cutoff
    std::cout << "\tTesting Cutoff..." << std::endl;
    assert(light.getCutoffAngle() == cutoff);
    float newCutoff = 30.0f;
    light.setCutoffAngle(newCutoff);
    assert(light.getCutoffAngle() == newCutoff);
    std::cout << "\tCutoff test OK." << std::endl;

    // Test esponente
    std::cout << "\tTesting Exponent..." << std::endl;
    assert(light.getExponent() == exponent);
    light.setExponent(5.0f);
    assert(light.getExponent() == 5.0f);
    std::cout << "\tExponent test OK." << std::endl;

    std::cout << "SpotLight tests passed!" << std::endl;
}



void DirectionalLightTest::test() {
    std::cout << "Running DirectionalLight tests..." << std::endl;

    glm::mat4 matrix = glm::mat4(1.0f);
    glm::vec3 ambient(0.1f);
    glm::vec3 diffuse(1.0f);
    glm::vec3 specular(1.0f);
    glm::vec3 dir(1.0f, 0.0f, 0.0f);

    DirectionalLight light("DirectionalLightTest", matrix, ambient, diffuse, specular, dir);

    std::cout << "\tTesting Direction..." << std::endl;
    assert(light.getDirection() == dir);

    glm::vec3 newDir(0.0f, 1.0f, 0.0f);
    light.setDirection(newDir);
    assert(light.getDirection() == newDir);
    std::cout << "\tDirection test OK." << std::endl;

    std::cout << "DirectionalLight tests passed!" << std::endl;
}




void OmnidirectionalLightTest::test() {
    std::cout << "Running OmnidirectionalLight tests..." << std::endl;

    glm::mat4 matrix = glm::mat4(1.0f);
    glm::vec3 ambient(0.1f);
    glm::vec3 diffuse(1.0f);
    glm::vec3 specular(1.0f);

    OmnidirectionalLight light("OmniLightTest", matrix, ambient, diffuse, specular);

    std::cout << "\tTesting Cutoff..." << std::endl;
    assert(light.getCutoffAngle() == 180.0f);
    std::cout << "\tCutoff test OK." << std::endl;

    std::cout << "OmnidirectionalLight tests passed!" << std::endl;
}

