#pragma once

//////////////
// #INCLUDE //
//////////////

// Standard libraries
#include <iostream>
#include <map>

// GLM
#include <glm/glm.hpp>
#include <glm/gtc/packing.hpp>

// Engine core
#include "Object.h"
#include "Node.h"

// Rendering resources
#include "Material.h"
#include "Mesh.h"
#include "Texture.h"

// Lighting
#include "Light.h"
#include "SpotLight.h"
#include "OmnidirectionalLight.h"
#include "DirectionalLight.h"


/////////////////
// OVO OBJECTS //
////////////////
/**
 * @class OvObject
 * @brief Base descriptor for OVO objects.
 */
class ENG_API OvObject {
public:
    /**
     * @enum Type
     * @brief Enumerates all supported OVO object types.
     */
    enum class Type : int {
        OBJECT = 0,      ///< Generic object
        NODE,            ///< Scene graph node
        OBJECT2D,        ///< 2D object
        OBJECT3D,        ///< 3D object
        LIST,            ///< Object list / container

        BUFFER,          ///< GPU buffer
        SHADER,          ///< Shader object
        TEXTURE,         ///< Texture object
        FILTER,          ///< Image filter
        MATERIAL,        ///< Material definition
        FBO,             ///< Framebuffer object
        QUAD,            ///< Quad primitive
        BOX,             ///< Box primitive
        SKYBOX,          ///< Skybox
        FONT,            ///< Font resource
        CAMERA,          ///< Camera object
        LIGHT,           ///< Light object
        BONE,            ///< Skeleton bone
        MESH,            ///< Mesh geometry
        SKINNED,         ///< Skinned mesh
        INSTANCED,       ///< Instanced geometry
        PIPELINE,        ///< Rendering pipeline
        EMITTER,         ///< Particle emitter
        ANIM,            ///< Animation data
        PHYSICS,         ///< Physics object
        LAST,            ///< Sentinel value
    };
};

/**
 * @class OvMesh
 * @brief Descriptor for mesh-specific subtypes.
 */
class ENG_API OvMesh {
public:
    /**
     * @enum Subtype
     * @brief Enumerates supported mesh subtypes.
     */
    enum class Subtype : int {
        DEFAULT = 0,     ///< Standard mesh
        NORMALMAPPED,   ///< Mesh with normal mapping
        TESSELLATED,    ///< Tessellated mesh
        LAST,           ///< Sentinel value
    };
};

/**
 * @class OvLight
 * @brief Descriptor for light-specific subtypes.
 *
 * Used to select the appropriate Light subclass
 * during OVO parsing.
 */
class ENG_API OvLight {
public:
    /**
     * @enum Subtype
     * @brief Enumerates supported light types.
     */
    enum class Subtype : int {
        OMNI = 0,        ///< Omnidirectional (point) light
        DIRECTIONAL,    ///< Directional (infinite) light
        SPOT,           ///< Spot light
        LAST,           ///< Sentinel value
    };
};

/////////////////
// OVO OBJECTS //
////////////////

/**
 * @class OvoReader
 * @brief OVO file parser and scene loader.
 *
 * OvoReader is responsible for reading binary .OVO files
 * and converting them into a scene graph composed of Nodes,
 * Meshes, Materials, Textures and Lights.
 *
 * The parsing is performed recursively, following the hierarchy
 * encoded in the file.
 */
class ENG_API OvoReader {
public:
    /**
     * @brief Loads an OVO file and builds the corresponding scene graph.
     *
     * @param file_path Path to the .ovo file.
     * @param texture_dir Directory containing texture files.
     * @return Pointer to the root Node of the loaded scene graph.
     */
    Node* readFile(const char* file_path, const char* texture_dir);

protected:
    /** Map of already parsed materials, indexed by name */
    std::map<std::string, Material*> m_materials;

    /**
     * @brief Recursively loads nodes from the OVO file.
     *
     * @param dat File pointer to the opened OVO file.
     * @param path Path to the OVO file (used for error reporting).
     * @return Pointer to the parsed Node.
     */
    Node* recursive_load(FILE* dat, const char* path);

    /**
     * @brief Parses a generic OBJECT chunk.
     *
     * @param data Pointer to the chunk data buffer.
     * @param position Current read offset inside the buffer.
     */
    void parse_object(char* data, unsigned int& position);

    /**
     * @brief Parses a MATERIAL chunk.
     *
     * @param data Pointer to the chunk data buffer.
     * @param position Current read offset inside the buffer.
     * @param texture_dir Base directory for texture files.
     * @return Pointer to the created Material.
     */
    Material* parse_material(char* data, unsigned int& position, const char* texture_dir);

    /**
     * @brief Parses a NODE chunk.
     *
     * @param data Pointer to the chunk data buffer.
     * @param position Current read offset inside the buffer.
     * @param n_children Output parameter for the number of child nodes.
     * @return Pointer to the created Node.
     */
    Node* parse_node(char* data, unsigned int& position, unsigned int* n_children);

    /**
     * @brief Parses a MESH chunk.
     *
     * @param data Pointer to the chunk data buffer.
     * @param position Current read offset inside the buffer.
     * @param n_children Output parameter for the number of child nodes.
     * @return Pointer to the created Mesh.
     */
    Mesh* parse_mesh(char* data, unsigned int& position, unsigned int* n_children);

    /**
     * @brief Parses a LIGHT chunk.
     *
     * Depending on the subtype, this method creates an
     * OmnidirectionalLight, DirectionalLight or SpotLight.
     *
     * @param data Pointer to the chunk data buffer.
     * @param position Current read offset inside the buffer.
     * @param n_children Output parameter for the number of child nodes.
     * @return Pointer to the created Light.
     */
    Light* parse_light(char* data, unsigned int& position, unsigned int* n_children);
};