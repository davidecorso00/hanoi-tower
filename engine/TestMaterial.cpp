#include "TestMaterial.h"
#include <iostream>
#include <cassert>
#include <glm/glm.hpp>

namespace MaterialTest {

    void test() {
        std::cout << "Running Material tests..." << std::endl;
        
        // Create a Material with full constructor
        glm::vec3 emission(0.1f, 0.2f, 0.3f);
        glm::vec3 ambient(0.2f, 0.3f, 0.4f);
        glm::vec3 diffuse(0.5f, 0.6f, 0.7f);
        glm::vec3 specular(0.8f, 0.9f, 1.0f);
        Material mat2("FullMaterial", emission, ambient, diffuse, specular, 10.0f, 0.5f);

        // Test getters
        std::cout << "\tTesting getters..." << std::endl;
        assert(mat2.getEmissionColor() == emission);
        assert(mat2.getAmbientColor() == ambient);
        assert(mat2.getDiffuseColor() == diffuse);
        assert(mat2.getSpecularColor() == specular);
        assert(mat2.getShininessExponent() == 10.0f);
        assert(mat2.getTransparencyFactor() == 0.5f);
        assert(mat2.getAssociatedTexture() == nullptr);
        std::cout << "\tGetters test OK." << std::endl;

        // Test setters
        std::cout << "\tTesting setters..." << std::endl;
        glm::vec3 newEmission(0.9f, 0.8f, 0.7f);
        mat2.setEmissionColor(newEmission);
        assert(mat2.getEmissionColor() == newEmission);

        mat2.setShininessExponent(5.0f);
        assert(mat2.getShininessExponent() == 5.0f);

        mat2.setTransparencyFactor(0.2f);
        assert(mat2.getTransparencyFactor() == 0.2f);
        std::cout << "\tSetters test OK." << std::endl;

        std::cout << "Material tests passed!" << std::endl;
    }
}