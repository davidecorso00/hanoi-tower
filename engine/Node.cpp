#include "Node.h"

#include <algorithm>
#include <iostream>

// ------------------------------------------------------------
// Constructor -> initializes parent to nullptr and matrix to identity.
// ------------------------------------------------------------
ENG_API Node::Node(const std::string& name)
    : Object(name), parentNode(nullptr), localTransformationMatrix(glm::mat4(1.0f))   // Identity matrix
{}

// ------------------------------------------------------------
// Local Matrix Getters/Setters
// ------------------------------------------------------------
// Return a copy of the local matrix
glm::mat4 Node::getLocalMatrix() const {
    return localTransformationMatrix;
}

// Replace the entire local transform
void Node::setLocalMatrix(const glm::mat4& newMatrix) {
    localTransformationMatrix = newMatrix;
}

// ------------------------------------------------------------
// Local Transformations
// ------------------------------------------------------------
// Apply: translation
void Node::translate(const glm::vec3& translationShift) {
    glm::mat4 updated = glm::translate(localTransformationMatrix, translationShift);
    localTransformationMatrix = updated;
}

// Apply: rotation (radians)
void Node::rotate(float angleDegrees, const glm::vec3& axis) {
    float radianti = glm::radians(angleDegrees);
    glm::mat4 result = glm::rotate(localTransformationMatrix, radianti, axis);
    localTransformationMatrix = result;
}

// Apply: scaling
void Node::scale(const glm::vec3& scaleFactor) {
    glm::mat4 result = glm::scale(localTransformationMatrix, scaleFactor);
    localTransformationMatrix = result;
}

// ------------------------------------------------------------
// World Matrix Calculation
// ------------------------------------------------------------
// Recursively "remembers" matrices from parents
glm::mat4 Node::getWorldMatrix() const {
    if (parentNode != nullptr) {
        glm::mat4 parentWorld = parentNode->getWorldMatrix();
        return parentWorld * localTransformationMatrix;
    }
    return localTransformationMatrix;
}

// ------------------------------------------------------------
// Rendering
// ------------------------------------------------------------
// Base void implementation: derived class will override this
void Node::render() {}

// ------------------------------------------------------------
// Scene Graph Hierarchy Handling
// ------------------------------------------------------------
Node* Node::getParentNode() const {
    return parentNode;
}

void Node::setParentNode(Node* newParentNodeToSet) {
    parentNode = newParentNodeToSet;
}

// Set child's parent and append to vector
void Node::addChildNode(Node* newChildNodeToAdd) {
    if (newChildNodeToAdd == nullptr) return;

    newChildNodeToAdd->setParentNode(this);
    childrenNodes.push_back(newChildNodeToAdd);
}

// Find and erase the child
void Node::removeChildNode(Node* childNodeToRemove) {
    if (!childNodeToRemove) return;

    auto result = std::remove(childrenNodes.begin(), childrenNodes.end(), childNodeToRemove);
    if (result != childrenNodes.end()) {
        childNodeToRemove->setParentNode(nullptr);
        childrenNodes.erase(result, childrenNodes.end());
    }
}

Node* Node::getChildByIndex(int x) const {
    if (x < 0 || x >= static_cast<int>(childrenNodes.size()))
        return nullptr;

    return childrenNodes[x];
}

unsigned int Node::getTotalChildren() const {
    return static_cast<unsigned int>(childrenNodes.size());
}

// ------------------------------------------------------------
// Recursive Search by Name
// ------------------------------------------------------------
Node* Node::findRecursiveNodeByName(const std::string& nodeNameToFind) {
    if (this->name == nodeNameToFind) {
        return this;
    }
        
    // Recursively check children
    for (Node* c : childrenNodes) {
        Node* found = c->findRecursiveNodeByName(nodeNameToFind);
        if (found != nullptr) {
            return found;
        }
    }

    // If the node is not found -> nullptr
    return nullptr;
}
