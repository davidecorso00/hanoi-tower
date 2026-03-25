#include "TestNode.h"
#include <iostream>
#include <cassert>
#include <glm/glm.hpp>

namespace NodeTest {

    void test() {
        std::cout << "Running Node tests..." << std::endl;

        // Create root node
        Node root("RootNode");

        // Test initial parent
        std::cout << "\tTesting initial parent..." << std::endl;
        assert(root.getParentNode() == nullptr);
        std::cout << "\tInitial parent test OK." << std::endl;

        // Create child nodes
        Node child1("Child1");
        Node child2("Child2");

        // Add children
        std::cout << "\tTesting add children..." << std::endl;
        root.addChildNode(&child1);
        root.addChildNode(&child2);

        assert(root.getTotalChildren() == 2);
        assert(root.getChildByIndex(0) == &child1);
        assert(root.getChildByIndex(1) == &child2);

        // Test parent pointer
        assert(child1.getParentNode() == &root);
        assert(child2.getParentNode() == &root);
        std::cout << "\tAdd children test OK." << std::endl;

        // Test findByName
        std::cout << "\tTesting find by name..." << std::endl;
        assert(root.findRecursiveNodeByName("Child1") == &child1);
        assert(root.findRecursiveNodeByName("Child2") == &child2);
        assert(root.findRecursiveNodeByName("NonExistent") == nullptr);
        std::cout << "\tFind by name test OK." << std::endl;

        // Test removeChild
        std::cout << "\tTesting remove child..." << std::endl;
        root.removeChildNode(&child1);
        assert(root.getTotalChildren() == 1);
        assert(root.getChildByIndex(0) == &child2);
        assert(child1.getParentNode() == nullptr);
        std::cout << "\tRemove child test OK." << std::endl;

        // Test transformations (local)
        std::cout << "\tTesting transformations..." << std::endl;
        glm::mat4 initialMatrix = child2.getLocalMatrix();
        child2.translate(glm::vec3(1.0f, 2.0f, 3.0f));
        child2.rotate(90.0f, glm::vec3(0.0f, 1.0f, 0.0f));
        child2.scale(glm::vec3(2.0f, 2.0f, 2.0f));
        glm::mat4 finalMatrix = child2.getLocalMatrix();
        assert(finalMatrix != initialMatrix);

        // Test world matrix with non-identity parent transform:
        // apply a translation to the root so that world != local.
        root.translate(glm::vec3(5.0f, 0.0f, 0.0f));
        glm::mat4 parentWorld = root.getWorldMatrix();
        glm::mat4 worldMatrix = child2.getWorldMatrix();
        glm::mat4 expectedWorld = parentWorld * child2.getLocalMatrix();

        assert(worldMatrix == expectedWorld);
        assert(worldMatrix != child2.getLocalMatrix());
        std::cout << "\tTransformations test OK." << std::endl;

        std::cout << "Node tests passed!" << std::endl;
    }
}
