#include "TestMesh.h"
#include <iostream>
#include <cassert>
#include <glm/glm.hpp>

namespace MeshTest {

void test() {
    std::cout << "Running Mesh tests..." << std::endl;

    Mesh mesh("TestMesh");

    // Test vertices
    std::vector<glm::vec3> vertices = { {0,0,0}, {1,0,0}, {0,1,0} };
    mesh.setVertices(vertices);
    assert(mesh.getVertices() == vertices);
    std::cout << "\tVertices setter/getter OK." << std::endl;

    // Test normals
    std::vector<glm::vec3> normals = { {0,0,1}, {0,0,1}, {0,0,1} };
    mesh.setNormals(normals);
    assert(mesh.getNormals() == normals);
    std::cout << "\tNormals setter/getter OK." << std::endl;

    // Test texture coordinates
    std::vector<glm::vec2> texCoords = { {0,0}, {1,0}, {0,1} };
    mesh.setTextureCoordinates(texCoords);
    assert(mesh.getTextureCoordinates() == texCoords);
    std::cout << "\tTexture coords setter/getter OK." << std::endl;

    // Test face vertices
    std::vector<std::vector<unsigned int>> faces = { {0,1,2} };
    mesh.setFaceVertices(faces);
    assert(mesh.getFaceVertices() == faces);
    std::cout << "\tFace vertices setter/getter OK." << std::endl;

    std::cout << "Mesh tests passed!" << std::endl;
}

} // namespace MeshTest