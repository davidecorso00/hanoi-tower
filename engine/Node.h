#pragma once

#include "Object.h"
#include "Lib.h"

#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"

#include <vector>

/**
 * @brief Base node of the scene graph, handling transformations and hierarchy.
 *
 * Each node can have children, a parent, and a local transformation matrix.
 * Derived classes such as Mesh or Light may override the render() method.
 */
class ENG_API Node : public Object {
public:
    /**
     * @brief Constructor with name.
     * @param name Name of the node.
     */
    Node(const std::string& name);

    /**
     * @brief Virtual destructor.
     */
    virtual ~Node() = default;

    // -------------------
    // Transformation Matrix
    // -------------------
    /**
     * @brief Returns the local transformation matrix of the node.
     * @return glm::mat4 local transformation matrix.
     */
    glm::mat4 getLocalMatrix() const;

    /**
     * @brief Sets the local transformation matrix of the node.
     * @param newMatrix New transformation matrix.
     */
    void setLocalMatrix(const glm::mat4& newMatrix);

    /**
     * @brief Translates the node.
     * @param translation Translation vector.
     */
    void translate(const glm::vec3& translation);

    /**
     * @brief Rotates the node around an axis.
     * @param angle Angle in degrees.
     * @param axis Axis vector.
     */
    void rotate(float angle, const glm::vec3& axis);

    /**
     * @brief Scales the node.
     * @param factor Scaling factor for x, y, z.
     */
    void scale(const glm::vec3& factor);

    /**
     * @brief Computes the final world matrix recursively.
     * @return glm::mat4 final world matrix.
     */
    glm::mat4 getWorldMatrix() const;

    // -------------------
    // Rendering
    // -------------------
    /**
     * @brief Renders the node. Derived classes may override this.
     */
    void render() override;

    // -------------------
    // Parent-Children Hierarchy
    // -------------------
    /**
     * @brief Returns the node's parent.
     * @return Parent node.
     */
    Node* getParentNode() const;

    /**
     * @brief Sets the parent of the node.
     * @param newParent New parent node.
     */
    void setParentNode(Node* newParent);

    /**
     * @brief Adds a child to the node.
     * @param child Child node to add.
     */
    void addChildNode(Node* child);

    /**
     * @brief Removes a child from the node.
     * @param child Child node to remove.
     */
    void removeChildNode(Node* child);

    /**
     * @brief Returns a child based on index.
     * @param index Child index.
     * @return Child node or nullptr if nonexistent.
     */
    Node* getChildByIndex(int index) const;

    /**
     * @brief Returns the number of children.
     * @return Number of children.
     */
    unsigned int getTotalChildren() const;

    /**
     * @brief Recursive search by node name.
     * @param nodeName Name to search for.
     * @return Found node or nullptr if not found.
     */
    Node* findRecursiveNodeByName(const std::string& nodeNameToFind);

private:
    Node* parentNode;                      ///< Pointer to the parent node
    std::vector<Node*> childrenNodes;      ///< List of child nodes
    glm::mat4 localTransformationMatrix;   ///< Local transformation matrix
};