#pragma once

#include <vector>
#include <glm/glm.hpp>

#include "Lib.h"
#include "Node.h"
#include "Material.h"

/**
 * @class Mesh
 * @brief Represents a 3D mesh in the scene.
 *
 * A mesh is a geometric object composed of vertices, normals,
 * texture coordinates and faces.
 */
class ENG_API Mesh : public Node {
public:
    /**
     * @brief Constructs a mesh with a given name.
     * @param name Name of the mesh.
     */
    Mesh(const std::string& meshName);

    /**
     * @brief Constructs a fully initialized mesh.
     *
     * @param name Name of the mesh.
     * @param matrix Initial transformation matrix.
     * @param faces Number of faces in the mesh.
     * @param vertices Number of vertices in the mesh.
     * @param material Pointer to the associated material.
     */
    Mesh(
        const std::string& meshName,
        glm::mat4 initTransformationMatrix,
        unsigned int numberOfFaces,
        unsigned int numOfVertices,
        Material* associatedMaterial
    );

    // ===================
    // Getters & Setters
    // ===================

    /** @return Vector containing all mesh vertices */
    const std::vector<glm::vec3>& getVertices() const;

    /**
     * @brief Sets the mesh vertices.
     * @param vertices New mesh vertices.
     */
    void setVertices(const std::vector<glm::vec3>& verticesToSet);



    /** @return Vector containing all vertex normals */
    const std::vector<glm::vec3>& getNormals() const;

    /**
     * @brief Sets the mesh normals.
     * @param normals New mesh normals.
     */
    void setNormals(const std::vector<glm::vec3>& normalsToSet);



    /** @return Vector containing all texture coordinates */
    const std::vector<glm::vec2>& getTextureCoordinates() const;

    /**
     * @brief Sets the texture coordinates.
     * @param textureCoords Texture coordinates.
     */
    void setTextureCoordinates(const std::vector<glm::vec2>& textureCoordinatesToSet);



    /** @return Vertex indices for each face */
    const std::vector<std::vector<unsigned int>>& getFaceVertices() const;

    /**
     * @brief Sets the face vertex indices.
     * @param faces Vector containing face definitions.
     */
    void setFaceVertices(const std::vector<std::vector<unsigned int>>& faceVerticesToSet);



    /** @return Pointer to the associated material */
    Material* getAssociatedMaterial() const;

    /**
     * @brief Assigns a material to the mesh.
     * @param material Pointer to the material.
     */
    void setAssociatedMaterial(Material* associatedMaterialToSet);



    /**
     * @brief Renders the mesh.
     *
     * Applies the associated material and draws the mesh
     * using OpenGL primitives.
     */
    void render() override;

    /**
     * @brief Renders geometry only (no material, no lighting state changes).
     *
     * Used for auxiliary passes such as planar shadows where only
     * the raw triangles are required.
     */
    void renderShadowGeometry() const;

private:
    std::vector<glm::vec3> meshVertices;                     ///< Mesh vertices
    std::vector<glm::vec3> meshNormals;                      ///< Vertex normals
    std::vector<glm::vec2> meshTextureCoordinates;           ///< Texture coordinates
    std::vector<std::vector<unsigned int>> faceVertices;     ///< Face vertex indices

    unsigned int totalMeshFaces;                                 ///< Total number of faces
    unsigned int totalMeshVertices;                              ///< Total number of vertices

    Material* associatedMaterial;                                      ///< Associated material
    glm::mat4 initTransformationMatrix;                                        ///< Local transformation matrix
};