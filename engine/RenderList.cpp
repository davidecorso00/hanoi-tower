#include "RenderList.h"

#include "Light.h"
#include "Mesh.h"

#include <GL/freeglut.h>
#include <glm/gtc/type_ptr.hpp>
#include <iostream>
#include <cmath>

namespace {
    // Height of the plane where shadows are projected (table/floor level).
    constexpr float SHADOW_PLANE_HEIGHT = 2.0f;

    /**
     * Builds a projection matrix that flattens geometry onto a plane from a given light position.
     * Formula based on the classic Red Book shadow matrix.
     */
    glm::mat4 buildShadowMatrix(const glm::vec4& plane, const glm::vec4& lightPos) {
        const float dot = plane.x * lightPos.x + plane.y * lightPos.y + plane.z * lightPos.z + plane.w * lightPos.w;

        glm::mat4 shadow(0.0f);

        shadow[0][0] = dot - lightPos.x * plane.x;
        shadow[1][0] = -lightPos.x * plane.y;
        shadow[2][0] = -lightPos.x * plane.z;
        shadow[3][0] = -lightPos.x * plane.w;

        shadow[0][1] = -lightPos.y * plane.x;
        shadow[1][1] = dot - lightPos.y * plane.y;
        shadow[2][1] = -lightPos.y * plane.z;
        shadow[3][1] = -lightPos.y * plane.w;

        shadow[0][2] = -lightPos.z * plane.x;
        shadow[1][2] = -lightPos.z * plane.y;
        shadow[2][2] = dot - lightPos.z * plane.z;
        shadow[3][2] = -lightPos.z * plane.w;

        shadow[0][3] = -lightPos.w * plane.x;
        shadow[1][3] = -lightPos.w * plane.y;
        shadow[2][3] = -lightPos.w * plane.z;
        shadow[3][3] = dot - lightPos.w * plane.w;

        return shadow;
    }
}

ENG_API RenderList::RenderList()
	: Object("RenderList")
{
}

RenderList::~RenderList() {
	clearInstances();
}

/**
* Converting a scene graph into a list of instances.
* @param nodeToAdd current node
* @param finalWorldMatrix previous node�s world matrix
*/
void RenderList::pass(Node* nodeToAdd, glm::mat4 finalWorldMatrix) {
	if (!nodeToAdd) {
		return;
	}

	glm::mat4 currentUpdated = finalWorldMatrix * nodeToAdd->getLocalMatrix();

	Instance instance;
	instance.nodePtr = nodeToAdd;
	instance.nodeWorldMatrix = currentUpdated;

	// Check for light
	// -> if the node is a light => push front
	// -> if the node is something else => push back
	if (dynamic_cast<Light*>(nodeToAdd) != nullptr){
		instances.push_front(instance);
	} else {
		instances.push_back(instance);
	}

	// Recursion
	for (unsigned int x = 0; x < nodeToAdd->getTotalChildren(); x++) {
		pass(nodeToAdd->getChildByIndex(x), currentUpdated);
	}
}

/**
* Call the render method for each instance in the list.
* @param M camera/view matrix
*/
void RenderList::render(glm::mat4 M) {
	constexpr int MAX_LIGHTS = 8;
	int activeLights = 0;

	// Determine if the Omni002 light is active and retrieve its world position
	bool omniShadowActive = false;
	glm::vec4 omniLightWorldPos(0.0f);
	for (const auto& instance : instances) {
		Light* lightNode = dynamic_cast<Light*>(instance.nodePtr);
		if (lightNode && instance.nodePtr->getName() == "Omni002") {
			// Use light components to understand if the light is effectively on (see keyboard toggle)
			glm::vec3 energy = glm::vec3(lightNode->getAmbientLightComponent()) +
				glm::vec3(lightNode->getDiffuseLightComponent()) +
				glm::vec3(lightNode->getSpecularLightComponent());

			if (glm::length(energy) > 0.001f) {
				omniLightWorldPos = instance.nodeWorldMatrix * glm::vec4(0.0f, 0.0f, 0.0f, 1.0f);
				omniShadowActive = true;
			}
			break;
		}
	}

	const glm::vec4 shadowPlane(0.0f, 1.0f, 0.0f, -SHADOW_PLANE_HEIGHT);
	glm::mat4 shadowMatrix(1.0f);
	if (omniShadowActive) {
		// Avoid degeneracy when the light lies on the shadow plane
		const float distanceFromPlane = std::abs(
			shadowPlane.x * omniLightWorldPos.x +
			shadowPlane.y * omniLightWorldPos.y +
			shadowPlane.z * omniLightWorldPos.z +
			shadowPlane.w * omniLightWorldPos.w
		);
		if (distanceFromPlane > 0.01f) {
			shadowMatrix = buildShadowMatrix(shadowPlane, omniLightWorldPos);
		}
		else {
			omniShadowActive = false;
		}
	}

	// Reset all OpenGL light slots at the beginning of the frame
	for (int x = 0; x < MAX_LIGHTS; x++) {
		glDisable(GL_LIGHT0 + x);
	}

	for (auto& instance : instances) {
		glm::mat4 modelViewMatrix = M * instance.nodeWorldMatrix;
		glMatrixMode(GL_MODELVIEW);
		glLoadMatrixf(glm::value_ptr(modelViewMatrix));

		Light* testLight = dynamic_cast<Light*>(instance.nodePtr);
		
		// Render light nodes
		if (testLight != nullptr) {
			if (activeLights < MAX_LIGHTS) {
				testLight->setLightId(GL_LIGHT0 + activeLights);
				testLight->render();
				activeLights = activeLights + 1;
			} else {
				// Exceeded hardware light limit
				testLight->setLightId(-1);
				std::cerr << "MAXIMUM NUMBER OF LIGHTS REACHED" << std::endl;
			}
		}
		
		// Render non-light nodes
		else {
			instance.nodePtr->render();
			// Render the node normally using its own render method

			// -------------------------------
			// Planar shadow rendering pass
			// (shadow projected by Omni002)
			// -------------------------------
			if (omniShadowActive) {
				// Ensure the node is a mesh before rendering its shadow
				if (auto* meshNode = dynamic_cast<Mesh*>(instance.nodePtr)) {

					// Compute the world transformation for the shadow
					// by applying the shadow projection matrix
					glm::mat4 shadowWorld = shadowMatrix * instance.nodeWorldMatrix;

					// Combine the shadow world matrix with the current view matrix
					glm::mat4 shadowModelView = M * shadowWorld;

					// Load the shadow model-view matrix into OpenGL
					glMatrixMode(GL_MODELVIEW);
					glLoadMatrixf(glm::value_ptr(shadowModelView));

					// Disable lighting and texturing to render a pure shadow
					glDisable(GL_LIGHTING);
					glDisable(GL_TEXTURE_2D);

					// Enable blending to achieve semi-transparent shadows
					glEnable(GL_BLEND);
					glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

					// Disable depth buffer writing to avoid z-fighting issues
					glDepthMask(GL_FALSE);

					// Apply polygon offset to push the shadow slightly
					// away from the surface and prevent depth artifacts
					glEnable(GL_POLYGON_OFFSET_FILL);
					glPolygonOffset(-1.0f, -1.0f);

					// Set shadow color (black with partial transparency)
					glColor4f(0.0f, 0.0f, 0.0f, 0.45f);

					// Render only the mesh geometry for the shadow
					meshNode->renderShadowGeometry();

					// -------------------------------
					// Restore OpenGL state
					// -------------------------------
					glDisable(GL_POLYGON_OFFSET_FILL);
					glDepthMask(GL_TRUE);
					glDisable(GL_BLEND);
					glEnable(GL_TEXTURE_2D);
					glEnable(GL_LIGHTING);

					// Restore default color
					glColor4f(1.0f, 1.0f, 1.0f, 1.0f);
				}
			}
		}
	}
}

void RenderList::render() {
	render(glm::mat4(1.0f));
}

void RenderList::clearInstances() {
	instances.clear();
}