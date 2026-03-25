#include "TestTexture.h"
#include "Texture.h"
#include <iostream>
#include <cassert>

namespace TextureTest {

    void test() {
        std::cout << "Running Texture tests..." << std::endl;

        // Test 1: Create a texture
        std::cout << "\tTesting texture creation..." << std::endl;
        Texture texture("TestTexture", "assets/test.png"); // Make sure this file exists
        assert(texture.getName() == "TestTexture");
        std::cout << "\tTexture creation test OK." << std::endl;

        // Test 2: Render bind (just ensure no crash)
        std::cout << "\tTesting render (bind)..." << std::endl;
        texture.render();
        std::cout << "\tRender test OK." << std::endl;

        // Test 3: Ensure multiple textures can be created
        std::cout << "\tTesting multiple textures..." << std::endl;
        Texture texture2("AnotherTexture", "assets/test.png");
        assert(texture2.getName() == "AnotherTexture");
        std::cout << "\tMultiple textures test OK." << std::endl;

        std::cout << "Texture tests passed!" << std::endl;
    }
}