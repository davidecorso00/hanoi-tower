//////////////
// INCLUDES //
//////////////
#define _CRT_SECURE_NO_WARNINGS
#include "OvoReader.h"
using namespace std;

// Macro for printing an OvMatrix4 to console
#define MAT2STR(m) cout << "   Matrix  . . . :  \t" << m[0][0] << "\t" << m[1][0] << "\t" << m[2][0] << "\t" << m[3][0] << std::endl \
                           << "                    \t" << m[0][1] << "\t" << m[1][1] << "\t" << m[2][1] << "\t" << m[3][1] << std::endl \
                           << "                    \t" << m[0][2] << "\t" << m[1][2] << "\t" << m[2][2] << "\t" << m[3][2] << std::endl \
                           << "                    \t" << m[0][3] << "\t" << m[1][3] << "\t" << m[2][3] << "\t" << m[3][3] << std::endl

/////////////
// CLASSES //
/////////////

/**
 * Loads an OVO file and builds the corresponding scene graph
 * @param file_path Path to the .ovo file
 * @param texture_dir Directory containing texture files
 * @return Pointer to the root Node of the loaded scene graph, or nullptr on error
 */
Node ENG_API* OvoReader::readFile(const char* file_path, const char* texture_dir) {
    FILE* fileHandle = fopen(file_path, "rb");

    // Open file
    if (fileHandle == nullptr) {
        std::cout << "[OvoReader] ERROR: Unable to open file '" << file_path << "'" << std::endl;
        return nullptr;
    }

    // Parse header chunks (materials first)
    unsigned int chunkId;
    unsigned int chunkSize;
    bool isHeader = true;

    while (isHeader) {
        fread(&chunkId, sizeof(unsigned int), 1, fileHandle);
        if (feof(fileHandle)) {
            fclose(fileHandle);
            return nullptr;
        }
        fread(&chunkSize, sizeof(unsigned int), 1, fileHandle);

        // Load whole chunk into memory
        char* chunkData = new char[chunkSize];

        if (fread(chunkData, sizeof(char), chunkSize, fileHandle) != chunkSize) {
            std::cout << "[OvoReader] ERROR: Unable to read from file '" << file_path << "'" << std::endl;
            fclose(fileHandle);
            delete[] chunkData;
            return nullptr;
        }

        // Parse chunk according to its type
        unsigned int position = 0;
        int rollbackOffset = 0;
        Material* material = nullptr;

        switch ((OvObject::Type)chunkId) {
        case OvObject::Type::OBJECT:
            parse_object(chunkData, position);
            break;

        case OvObject::Type::MATERIAL:
            material = parse_material(chunkData, position, texture_dir);
            if (material) {
                m_materials.insert(make_pair(material->getName(), material));
            }
            break;

        case OvObject::Type::NODE:
        case OvObject::Type::MESH:
        case OvObject::Type::LIGHT:
        case OvObject::Type::BONE:
        case OvObject::Type::SKINNED:
            // Header parsing complete, now start parsing scene graph nodes
            // Rollback file pointer to re-read this chunk in recursive_load
            isHeader = false;
            rollbackOffset = -1 * static_cast<int>(chunkSize + 8);
            fseek(fileHandle, rollbackOffset, SEEK_CUR);
            break;

        default:
            std::cout << "[OvoReader] ERROR: Corrupted or invalid data in file '" << file_path << "'" << std::endl;
            fclose(fileHandle);
            delete[] chunkData;
            return nullptr;
        }

        // Release chunk memory
        delete[] chunkData;
    }

    // Recursively load scene graph starting from root node
    Node* rootNode = recursive_load(fileHandle, file_path);
    fclose(fileHandle);
    std::cout << "[OvoReader] OVO file parsed successfully" << std::endl;

    return rootNode;
}

/**
 * Recursively loads nodes from the OVO file
 * @param fileHandle File pointer to the opened OVO file
 * @param path Path to the OVO file (used for error reporting)
 * @return Pointer to the parsed Node, or nullptr on error
 */
Node ENG_API* OvoReader::recursive_load(FILE* fileHandle, const char* path) {
    unsigned int chunkId;
    unsigned int chunkSize;
    fread(&chunkId, sizeof(unsigned int), 1, fileHandle);
    if (feof(fileHandle)) {
        return nullptr;
    }
    fread(&chunkSize, sizeof(unsigned int), 1, fileHandle);

    char* chunkData = new char[chunkSize];

    if (fread(chunkData, sizeof(char), chunkSize, fileHandle) != chunkSize) {
        std::cout << "[OvoReader] ERROR: Unable to read from file '" << path << "'" << std::endl;
        fclose(fileHandle);
        delete[] chunkData;
        return nullptr;
    }

    unsigned int position = 0;
    unsigned int numChildren = 0;
    Node* currentNode = nullptr;

    switch ((OvObject::Type)chunkId) {
    case OvObject::Type::NODE:
        currentNode = parse_node(chunkData, position, &numChildren);
        break;

    case OvObject::Type::MESH:
        currentNode = parse_mesh(chunkData, position, &numChildren);
        break;

    case OvObject::Type::LIGHT:
        currentNode = parse_light(chunkData, position, &numChildren);
        break;

    case OvObject::Type::BONE:
    case OvObject::Type::SKINNED:
        // Bone and skinned mesh types are skipped (not supported)
        break;

    default:
        std::cout << "[OvoReader] ERROR: Corrupted or invalid data in file '" << path << "'" << std::endl;
        fclose(fileHandle);
        delete[] chunkData;
        return nullptr;
    }

    delete[] chunkData;

    // Recursively parse child nodes
    if (currentNode != nullptr) {
        for (unsigned int childIndex = 0; childIndex < numChildren; childIndex++) {
            Node* childNode = recursive_load(fileHandle, path);
            if (childNode != nullptr) {
                currentNode->addChildNode(childNode);
            }
        }
    }

    return currentNode;
}

/**
 * Parses a generic OBJECT chunk (currently only reads version ID)
 * @param data Pointer to the chunk data buffer
 * @param position Current read offset inside the buffer (updated by reference)
 */
void ENG_API OvoReader::parse_object(char* data, unsigned int& position) {
    unsigned int versionId;
    memcpy(&versionId, data + position, sizeof(unsigned int));
    position += sizeof(unsigned int);
}

/**
 * Parses a MATERIAL chunk and creates a Material object
 * @param data Pointer to the chunk data buffer
 * @param position Current read offset inside the buffer (updated by reference)
 * @param texture_dir Base directory for texture files
 * @return Pointer to the created Material, or nullptr on error
 */
Material ENG_API* OvoReader::parse_material(char* data, unsigned int& position, const char* texture_dir) {
    char materialName[FILENAME_MAX];
    strcpy(materialName, data + position);
    position += strlen(materialName) + 1;

    // Material properties
    glm::vec3 emission;
    glm::vec3 albedo;
    float roughness;
    float metalness;
    float transparency;

    memcpy(&emission, data + position, sizeof(glm::vec3));
    position += sizeof(glm::vec3);
    memcpy(&albedo, data + position, sizeof(glm::vec3));
    position += sizeof(glm::vec3);
    memcpy(&roughness, data + position, sizeof(float));
    position += sizeof(float);
    memcpy(&metalness, data + position, sizeof(float));
    position += sizeof(float);
    memcpy(&transparency, data + position, sizeof(float));
    position += sizeof(float);

    std::cout << "[OvoReader] Parsing Material: '" << materialName << "' (transparency: " << transparency << ")" << std::endl;

    // Texture filenames
    char albedoTexture[FILENAME_MAX];
    strcpy(albedoTexture, data + position);
    position += (unsigned int)strlen(albedoTexture) + 1;

    char normalMapTexture[FILENAME_MAX];
    strcpy(normalMapTexture, data + position);
    position += (unsigned int)strlen(normalMapTexture) + 1;

    char heightMapTexture[FILENAME_MAX];
    strcpy(heightMapTexture, data + position);
    position += (unsigned int)strlen(heightMapTexture) + 1;

    char roughnessTexture[FILENAME_MAX];
    strcpy(roughnessTexture, data + position);
    position += (unsigned int)strlen(roughnessTexture) + 1;

    char metalnessTexture[FILENAME_MAX];
    strcpy(metalnessTexture, data + position);
    position += (unsigned int)strlen(metalnessTexture) + 1;

    // Create Material object
    float shininess = pow(1.0f - roughness, 4) * 128.0f;
    glm::vec4 specularVec4 = glm::vec4(glm::mix(glm::vec3(0.04f), albedo, metalness), 0.0f);
    glm::vec4 emissionVec4 = glm::vec4(emission, 0.0f);
    glm::vec4 albedoVec4 = glm::vec4(albedo, 0.0f);

    Material* material = new Material(materialName, emissionVec4, albedoVec4 * 0.1f, albedoVec4, specularVec4, shininess, transparency);

    // Load texture if specified (with path fix)
    if (std::string{ albedoTexture } != "[none]") {
        std::string texturePath = texture_dir;
        // Ensure path ends with directory separator
        if (!texturePath.empty() && texturePath.back() != '/' && texturePath.back() != '\\') {
            texturePath += "/";
        }
        texturePath += albedoTexture;

        std::cout << "   [Texture] Loading Albedo: " << texturePath << std::endl;

        Texture* texture = new Texture{ albedoTexture, texturePath };
        material->setAssociatedTexture(texture);
    }

    return material;
}

/**
 * Parses a NODE chunk and creates a Node object
 * @param data Pointer to the chunk data buffer
 * @param position Current read offset inside the buffer (updated by reference)
 * @param numChildren Output parameter for the number of child nodes
 * @return Pointer to the created Node
 */
Node ENG_API* OvoReader::parse_node(char* data, unsigned int& position, unsigned int* numChildren) {
    char nodeName[FILENAME_MAX];
    strncpy(nodeName, data + position, FILENAME_MAX - 1);
    nodeName[FILENAME_MAX - 1] = '\0';
    position += (unsigned int)strlen(nodeName) + 1;

    std::cout << "[OvoReader] Node found: '" << nodeName << "'" << std::endl;

    // Node transformation matrix
    glm::mat4 transformMatrix;
    memcpy(&transformMatrix, data + position, sizeof(glm::mat4));
    position += sizeof(glm::mat4);

    // Number of child nodes
    unsigned int children;
    memcpy(&children, data + position, sizeof(unsigned int));
    *numChildren = children;
    position += sizeof(unsigned int);

    // Optional target node name (not used in current implementation)
    char targetName[FILENAME_MAX];
    strcpy(targetName, data + position);
    position += (unsigned int)strlen(targetName) + 1;

    Node* node = new Node{ nodeName };
    node->setLocalMatrix(transformMatrix);
    return node;
}

/**
 * Parses a MESH chunk and creates a Mesh object
 * @param data Pointer to the chunk data buffer
 * @param position Current read offset inside the buffer (updated by reference)
 * @param numChildren Output parameter for the number of child nodes
 * @return Pointer to the created Mesh, or nullptr on error
 */
Mesh ENG_API* OvoReader::parse_mesh(char* data, unsigned int& position, unsigned int* numChildren) {
    // Mesh name
    char meshName[FILENAME_MAX];
    strcpy(meshName, data + position);
    position += (unsigned int)strlen(meshName) + 1;

    // Mesh transformation matrix
    glm::mat4 transformMatrix;
    memcpy(&transformMatrix, data + position, sizeof(glm::mat4));
    position += sizeof(glm::mat4);

    // Number of child nodes
    unsigned int children;
    memcpy(&children, data + position, sizeof(unsigned int));
    *numChildren = children;
    position += sizeof(unsigned int);

    // Optional target node name, or [none] if not used
    char targetName[FILENAME_MAX];
    strcpy(targetName, data + position);
    position += (unsigned int)strlen(targetName) + 1;

    // Mesh subtype
    unsigned char subtype;
    memcpy(&subtype, data + position, sizeof(unsigned char));
    char subtypeName[FILENAME_MAX];
    switch ((OvMesh::Subtype)subtype) {
    case OvMesh::Subtype::DEFAULT:
        strcpy(subtypeName, "standard");
        break;
    case OvMesh::Subtype::NORMALMAPPED:
        strcpy(subtypeName, "normal-mapped");
        break;
    case OvMesh::Subtype::TESSELLATED:
        strcpy(subtypeName, "tessellated");
        break;
    default:
        strcpy(subtypeName, "UNDEFINED");
    }
    position += sizeof(unsigned char);

    // Material name
    char materialName[FILENAME_MAX];
    strcpy(materialName, data + position);
    position += (unsigned int)strlen(materialName) + 1;

    std::cout << "[OvoReader] Mesh found: '" << meshName << "' -> Material: '" << materialName << "'" << std::endl;

    // Mesh bounding sphere radius
    float boundingRadius;
    memcpy(&boundingRadius, data + position, sizeof(float));
    position += sizeof(float);

    // Mesh bounding box minimum corner
    glm::vec3 boundingBoxMin;
    memcpy(&boundingBoxMin, data + position, sizeof(glm::vec3));
    position += sizeof(glm::vec3);

    // Mesh bounding box maximum corner
    glm::vec3 boundingBoxMax;
    memcpy(&boundingBoxMax, data + position, sizeof(glm::vec3));
    position += sizeof(glm::vec3);

    // Optional physics properties (not used in engine but necessary to advance file pointer)
    unsigned char hasPhysics;
    memcpy(&hasPhysics, data + position, sizeof(unsigned char));
    position += sizeof(unsigned char);

    if (hasPhysics) {
        struct PhysicsProperties {
            // Pay attention to 16-byte alignment (use padding)
            unsigned char type;
            unsigned char continuousCollisionDetection;
            unsigned char collideWithRigidBodies;
            unsigned char hullType;

            // Vector data
            glm::vec3 massCenter;

            // Mesh properties
            float mass;
            float staticFriction;
            float dynamicFriction;
            float bounciness;
            float linearDamping;
            float angularDamping;
            unsigned int numHulls;
            unsigned int padding;

            // Pointers (not used)
            void* physicsObject;
            void* hull;
        };
        PhysicsProperties physicsProps;
        memcpy(&physicsProps, data + position, sizeof(PhysicsProperties));
        position += sizeof(PhysicsProperties);

        // Skip hull data if present
        if (physicsProps.numHulls > 0) {
            for (unsigned int hullIndex = 0; hullIndex < physicsProps.numHulls; hullIndex++) {
                // Number of vertices in hull
                unsigned int numVertices;
                memcpy(&numVertices, data + position, sizeof(unsigned int));
                position += sizeof(unsigned int);

                // Number of faces in hull
                unsigned int numFaces;
                memcpy(&numFaces, data + position, sizeof(unsigned int));
                position += sizeof(unsigned int);

                // Hull centroid (skip)
                position += sizeof(glm::vec3);

                // Skip hull vertices
                for (unsigned int vertexIndex = 0; vertexIndex < numVertices; vertexIndex++) {
                    position += sizeof(glm::vec3);
                }

                // Skip hull faces
                for (unsigned int faceIndex = 0; faceIndex < numFaces; faceIndex++) {
                    position += sizeof(unsigned int) * 3;
                }
            }
        }
    }

    // Number of LOD levels (only first LOD is loaded)
    unsigned int numLODs;
    memcpy(&numLODs, data + position, sizeof(unsigned int));
    position += sizeof(unsigned int);
    // Warning: always loading the first level of detail (always available)

    // Number of vertices
    unsigned int numVertices;
    memcpy(&numVertices, data + position, sizeof(unsigned int));
    position += sizeof(unsigned int);

    // Number of faces
    unsigned int numFaces;
    memcpy(&numFaces, data + position, sizeof(unsigned int));
    position += sizeof(unsigned int);

    // Vertex data containers
    std::vector<glm::vec3> vertexData;
    std::vector<glm::vec3> normals;
    std::vector<glm::vec2> textureCoords;

    // Parse vertex data
    for (unsigned int vertexIndex = 0; vertexIndex < numVertices; vertexIndex++) {
        // Vertex coordinates
        glm::vec3 vertex;
        memcpy(&vertex, data + position, sizeof(glm::vec3));
        position += sizeof(glm::vec3);

        // Vertex normal (packed format)
        unsigned int normalData;
        memcpy(&normalData, data + position, sizeof(unsigned int));
        glm::vec4 normal = glm::unpackSnorm3x10_1x2(normalData);
        position += sizeof(unsigned int);

        // Texture coordinates (packed format)
        unsigned int textureData;
        memcpy(&textureData, data + position, sizeof(unsigned int));
        glm::vec2 uv = glm::unpackHalf2x16(textureData);
        position += sizeof(unsigned int);

        // Tangent vector (skip)
        position += sizeof(unsigned int);

        vertexData.push_back(vertex);
        normals.push_back(normal);
        textureCoords.push_back(uv);
    }

    // Parse face indices (each face is composed of three vertices)
    std::vector<std::vector<unsigned int>> facesData;

    for (unsigned int faceIndex = 0; faceIndex < numFaces; faceIndex++) {
        unsigned int faceIndices[3];
        memcpy(faceIndices, data + position, sizeof(unsigned int) * 3);
        position += sizeof(unsigned int) * 3;
        std::vector<unsigned int> faceArray = { faceIndices[0], faceIndices[1], faceIndices[2] };
        facesData.push_back(faceArray);
    }

    // Find material by name
    auto materialIterator = m_materials.find(materialName);
    if (materialIterator == m_materials.end()) {
        std::cout << "[OvoReader] ERROR: Material '" << materialName << "' not found in file" << std::endl;
        return nullptr;
    }

    // Create mesh with parsed data
    Mesh* mesh = new Mesh{ meshName, transformMatrix, numFaces, numVertices, materialIterator->second };
    mesh->setVertices(vertexData);
    mesh->setNormals(normals);
    mesh->setTextureCoordinates(textureCoords);
    mesh->setFaceVertices(facesData);

    std::cout << "   -> Vertices: " << numVertices << ", Faces: " << numFaces << std::endl;

    return mesh;
}

/**
 * Parses a LIGHT chunk and creates an appropriate Light subclass
 * @param data Pointer to the chunk data buffer
 * @param position Current read offset inside the buffer (updated by reference)
 * @param numChildren Output parameter for the number of child nodes
 * @return Pointer to the created Light (OmnidirectionalLight, DirectionalLight, or SpotLight)
 */
Light ENG_API* OvoReader::parse_light(char* data, unsigned int& position, unsigned int* numChildren) {
    // Light name
    char lightName[FILENAME_MAX];
    strcpy(lightName, data + position);
    position += (unsigned int)strlen(lightName) + 1;

    // Light transformation matrix
    glm::mat4 transformMatrix;
    memcpy(&transformMatrix, data + position, sizeof(glm::mat4));
    position += sizeof(glm::mat4);

    // Number of child nodes
    unsigned int children;
    memcpy(&children, data + position, sizeof(unsigned int));
    *numChildren = children;
    position += sizeof(unsigned int);

    // Optional target node name
    char targetName[FILENAME_MAX];
    strcpy(targetName, data + position);
    position += (unsigned int)strlen(targetName) + 1;

    // Light subtype (0 = omni, 1 = directional, 2 = spot)
    unsigned char subtype;
    memcpy(&subtype, data + position, sizeof(unsigned char));
    char subtypeName[FILENAME_MAX];
    switch ((OvLight::Subtype)subtype) {
    case OvLight::Subtype::DIRECTIONAL:
        strcpy(subtypeName, "directional");
        break;
    case OvLight::Subtype::OMNI:
        strcpy(subtypeName, "omni");
        break;
    case OvLight::Subtype::SPOT:
        strcpy(subtypeName, "spot");
        break;
    default:
        strcpy(subtypeName, "UNDEFINED");
    }
    position += sizeof(unsigned char);

    std::cout << "[OvoReader] Light found: '" << lightName << "' (type: " << subtypeName << ")" << std::endl;

    // Light color (used for ambient, diffuse, and specular)
    glm::vec3 color;
    memcpy(&color, data + position, sizeof(glm::vec3));
    position += sizeof(glm::vec3);

    // Light radius (only for Omni and Spot lights)
    float radius;
    memcpy(&radius, data + position, sizeof(float));
    position += sizeof(float);

    // Light direction (only for Spot lights)
    glm::vec3 direction;
    memcpy(&direction, data + position, sizeof(glm::vec3));
    position += sizeof(glm::vec3);

    // Cutoff angle (only for Spot lights)
    float cutoffAngle;
    memcpy(&cutoffAngle, data + position, sizeof(float));
    position += sizeof(float);

    // Spot exponent
    float spotExponent;
    memcpy(&spotExponent, data + position, sizeof(float));
    position += sizeof(float);

    // Shadow casting flag (1 = yes, 0 = no)
    unsigned char castShadows;
    memcpy(&castShadows, data + position, sizeof(unsigned char));
    position += sizeof(unsigned char);

    // Volumetric light flag (1 = yes, 0 = no)
    unsigned char isVolumetric;
    memcpy(&isVolumetric, data + position, sizeof(unsigned char));
    position += sizeof(unsigned char);

    // Create appropriate light type based on subtype
    Light* light = nullptr;
    float attenuation = 0.0f;

    if ((OvLight::Subtype)subtype == OvLight::Subtype::OMNI) {
        attenuation = glm::clamp(1.0f / radius, 0.0f, 1.0f);
        OmnidirectionalLight* omniLight = new OmnidirectionalLight(
            lightName, transformMatrix, color, color, color
        );
        light = omniLight;
    }
    else if ((OvLight::Subtype)subtype == OvLight::Subtype::DIRECTIONAL) {
        DirectionalLight* directionalLight = new DirectionalLight(
            lightName, transformMatrix, color, color, color, direction
        );
        light = directionalLight;
    }
    else if ((OvLight::Subtype)subtype == OvLight::Subtype::SPOT) {
        attenuation = glm::clamp(10.0f / radius, 0.0f, 1.0f);
        SpotLight* spotLight = new SpotLight(
            lightName, transformMatrix, color, color, color, direction, cutoffAngle, spotExponent
        );
        light = spotLight;
    }

    return light;
}
