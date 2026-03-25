#include "Mesh.h"

#include <GL/freeglut.h>
#include <iostream>

/**
 * @file Mesh.cpp
 * @brief Implementation of the Mesh class.
 */

// ------------------------------------------------------------------
// Constructors
// ------------------------------------------------------------------
Mesh::Mesh(const std::string& meshName) : Node(meshName) { }

Mesh::Mesh(
    const std::string& meshName,
    glm::mat4 initTransformationMatrix,
    unsigned int numberOfFaces,
    unsigned int numOfVertices,
    Material* associatedMaterial
)
    : Node(meshName), initTransformationMatrix(initTransformationMatrix), totalMeshFaces(numberOfFaces), totalMeshVertices(numOfVertices), associatedMaterial(associatedMaterial) { this->setLocalMatrix(initTransformationMatrix); }



// ------------------------------------------------------------------
// Getters & Setters
// ------------------------------------------------------------------
const std::vector<glm::vec3>& Mesh::getVertices() const {
    return meshVertices;
}

void Mesh::setVertices(const std::vector<glm::vec3>& verticesToSet) {
    meshVertices = verticesToSet;
}



const std::vector<std::vector<unsigned int>>& Mesh::getFaceVertices() const {
    return faceVertices;
}

void Mesh::setFaceVertices(const std::vector<std::vector<unsigned int>>& faceVerticesToSet) {
    faceVertices = faceVerticesToSet;
}



const std::vector<glm::vec3>& Mesh::getNormals() const {
    return meshNormals;
}

void Mesh::setNormals(const std::vector<glm::vec3>& normalsToSet) {
    meshNormals = normalsToSet;
}



const std::vector<glm::vec2>& Mesh::getTextureCoordinates() const {
    return meshTextureCoordinates;
}

void Mesh::setTextureCoordinates(const std::vector<glm::vec2>& textureCoordinatesToSet) {
    meshTextureCoordinates = textureCoordinatesToSet;
}



Material* Mesh::getAssociatedMaterial() const {
    return associatedMaterial;
}

void Mesh::setAssociatedMaterial(Material* associatedMaterialToSet) {
    this->associatedMaterial = associatedMaterialToSet;
}



// ------------------------------------------------------------------
// Rendering
// ------------------------------------------------------------------
/**
 * @brief Renders the mesh using OpenGL pipeline.
 *
 * If a material is associated, it is applied before rendering.
 * Otherwise, a default rendering state is used.
 */

/**
* 
* STEPS
* 1. Apply the material if it's not nullptr
* 2. Apply the geometry
* 3. If no material iis applied, restore the OpenGL state
* 
*/
void Mesh::render() {
    // 1
    const bool useMaterial = (associatedMaterial != nullptr);

    if (useMaterial) {
        associatedMaterial->render();
    }
    else {
        glDisable(GL_LIGHTING);
        glDisable(GL_TEXTURE_2D);
        glColor3f(1.0f, 1.0f, 1.0f);
    }

    // 2
    if (!meshVertices.empty()) {
        if (!faceVertices.empty()) {

            glBegin(GL_TRIANGLES);

            for (size_t faceIdx = 0; faceIdx < faceVertices.size(); ++faceIdx) {
                const auto& face = faceVertices[faceIdx];

                for (size_t i = 0; i < face.size(); ++i) {
                    const unsigned int idx = face[i];

                    if (idx >= meshVertices.size()) continue;

                    if (idx < meshNormals.size()) {
                        const glm::vec3& n = meshNormals[idx];
                        glNormal3f(n.x, n.y, n.z);
                    }

                    if (idx < meshTextureCoordinates.size()) {
                        const glm::vec2& t = meshTextureCoordinates[idx];
                        glTexCoord2f(t.x, t.y);
                    }

                    const glm::vec3& v = meshVertices[idx];
                    glVertex3f(v.x, v.y, v.z);
                }
            }

            glEnd();
        }
    }

    // 3
    if (!useMaterial) {
        glEnable(GL_LIGHTING);
    }
}

/**
 * @brief Draws only the mesh triangles without applying a material.
 *
 * Lighting and texturing should be configured by the caller
 * before invoking this method.
 */
void Mesh::renderShadowGeometry() const {
    if (meshVertices.empty() || faceVertices.empty()) {
        return;
    }

    glBegin(GL_TRIANGLES);

    for (const auto& face : faceVertices) {
        for (unsigned int idx : face) {
            if (idx >= meshVertices.size()) continue;

            if (idx < meshNormals.size()) {
                const glm::vec3& n = meshNormals[idx];
                glNormal3f(n.x, n.y, n.z);
            }

            const glm::vec3& v = meshVertices[idx];
            glVertex3f(v.x, v.y, v.z);
        }
    }

    glEnd();
}