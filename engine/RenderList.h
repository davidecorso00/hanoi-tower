#pragma once

#include "Lib.h"

#include "Object.h"
#include "Node.h"

#include <list>
#include <glm/glm.hpp>

/**
 * @brief Render queue used to store scene nodes ready for rendering.
 *
 * The RenderList collects nodes together with their final world-space
 * transformation matrices. It decouples scene traversal from the actual
 * rendering phase.
 */
class ENG_API RenderList : public Object {
	/**
	 * @brief Represents a renderable instance.
	 *
	 * Each instance stores a pointer to a node and its corresponding
	 * world-space transformation matrix.
	 */
	struct Instance {
		Node* nodePtr;                ///< Pointer to the scene node
		glm::mat4 nodeWorldMatrix;	  ///< Final world-space transformation
	};
	std::list<Instance> instances;    ///< List of renderable instances

public:
	/**
	* @brief Default constructor.
	*/
	RenderList();

	/**
	 * @brief Destructor.
	 */
	~RenderList();

	/**
	 * @brief Adds a node instance to the render list.
	 *
	 * @param nodeToAdd Pointer to the scene node to be rendered.
	 * @param finalWorldMatrix Final world-space transformation matrix of the node.
	 */
	void pass(Node* nodeToAdd, glm::mat4 finalWorldMatrix);
	
	/**
	 * @brief Renders the list using the default view matrix.
	 *
	 * Overrides Object::render().
	 */
	void render() override;

	/**
	 * @brief Renders all stored instances using the given view matrix.
	 *
	 * @param M View transformation matrix.
	 */
	void render(glm::mat4 M);

	/**
	 * @brief Clears all stored instances.
	 */
	void clearInstances();
};