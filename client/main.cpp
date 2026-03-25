//////////////
// INCLUDES //
//////////////

#include "Engine.h"
#include "Camera.h"
#include "Light.h"
#include "Mesh.h"
#include "Material.h"
#include "OmnidirectionalLight.h"
#include "OvoReader.h"
#include "PerspectiveCamera.h"

#include "Hanoi.h"

#include <iostream>
#include <vector>
#include <string>
#include <algorithm>
#include <limits>
#include <cstdlib>
#include <optional>
#include <cmath>
#include <array>
#include <iomanip>
#include <chrono>

// Global variables
Eng::Base* engine;
Camera* camera;
RenderList* renderList;
Node* sceneRoot;
OvoReader ovoReader{};
Node* tableNode;
bool wireframe = false;
bool showMenu = true;

// Game logic instance
Hanoi* hanoiGame = nullptr;

glm::mat4 mainCameraHome{ 1.0f };
bool isRotationMode, isPresetView = false;

// Variables for Omni002 light toggle functionality
bool omniLightEnabled = true;  // true = light on, false = light off
glm::vec4 omniLightOriginalAmbient{ 0.0f };
glm::vec4 omniLightOriginalDiffuse{ 0.0f };
glm::vec4 omniLightOriginalSpecular{ 0.0f };
bool omniLightValuesSaved = false;  // flag to track if original light values have been saved

// Variable to display "NEW GAME STARTED" message
bool showNewGameMessage = false;
std::chrono::time_point<std::chrono::steady_clock> newGameMessageStartTime;

/**
 * Displays a game message centered on screen with specified color and vertical offset
 * @param text Message text to display
 * @param yOffset Vertical offset from screen center (positive = up, negative = down)
 * @param r Red component (0.0-1.0)
 * @param g Green component (0.0-1.0)
 * @param b Blue component (0.0-1.0)
 */
void displayGameMessage(std::string text, float yOffset, float r, float g, float b) {
    int windowWidth = engine->getWindowWidth();
    int windowHeight = engine->getWindowHeight();
    int textWidth = engine->getTextWidth(text);

    float x = (windowWidth - textWidth) / 2.0f;
    float y = (windowHeight / 2.0f) + yOffset;
    engine->addString(x, y, text, r, g, b);
}

/**
 * Handles special key input (arrow keys) by delegating to game logic
 */
void specialCallback(int key, int x, int y) {
    if (hanoiGame) {
        hanoiGame->specialCallback(key, x, y);
    }
}

/**
 * Main display callback - called every frame to render the scene
 */
void displayCallback() {
    static float angle = 0.0f;
    angle += 0.5f;

    // Update held disc animation (hover)
    if (hanoiGame) {
        hanoiGame->updateHeldDiscVisual(angle);
    }

    // Prepare render list
    renderList->clearInstances();
    renderList->pass(sceneRoot, glm::mat4(1.0f));

    engine->setRenderList(renderList);
    engine->setMainCamera(camera);
    engine->clearScreenText();

    // Update game timer display
    if (hanoiGame) {
        float elapsedTime = hanoiGame->getElapsedTime();
        engine->setGameTimer(elapsedTime);
        int moveCount = hanoiGame->getMoveCount();
        engine->setMoveCount(moveCount);
    }

    // Display menu instructions
    if (showMenu) {
        engine->addToScreenText(" ========== MENU (H to hide) ==========");
        engine->addToScreenText(" | Switch Move/Rotate Camera:  [P]    |");
        engine->addToScreenText(" | Movement/Rotation Camera:  [WASD]  |");
        engine->addToScreenText(" | Switch to Main Camera:      [0]    |");
        engine->addToScreenText(" | Switch Camera Presets:      [12]   |");
        engine->addToScreenText(" | Switch Peg:             [ARROW R/L]|");
        engine->addToScreenText(" | Pick/Release Disk:      [ARROW U/D]|");
        engine->addToScreenText(" | Toggle Omni/Wireframe:     [L/F]   |");
        engine->addToScreenText(" | Undo/Redo Move:            [U/O]   |");
        engine->addToScreenText(" | Reset/Quit Game:           [R/Q]   |");
        engine->addToScreenText(" ======================================");
    }
    else {
        engine->addToScreenText(" ========== MENU (H to show) ==========");
    }

    // Display victory message if game is won
    if (hanoiGame && hanoiGame->isGameWon()) {
        float elapsedTime = hanoiGame->getElapsedTime();
        int seconds = static_cast<int>(elapsedTime);
        std::string victoryText;
        
        if (seconds < 150) {
            victoryText = "YOU WON!!! - " + std::to_string(seconds) + "s, too easy for you!";
        } else {
            victoryText = "YOU WON!!! - " + std::to_string(seconds) + "s, you can improve!";
        }
        
        displayGameMessage(victoryText, 0.0f, 0.2f, 1.0f, 0.2f);
        displayGameMessage("Press [Q] to Quit or [R] to Play Again!", -30.0f, 1.0f, 1.0f, 1.0f);
    }

    // Display "NEW GAME STARTED" message with blinking effect
    if (showNewGameMessage) {
        auto currentTime = std::chrono::steady_clock::now();
        std::chrono::duration<float> elapsedTime = currentTime - newGameMessageStartTime;
        float elapsedSeconds = elapsedTime.count();
        
        // Hide message after 2 seconds
        if (elapsedSeconds >= 2.0f) {
            showNewGameMessage = false;
        } else {
            // Blink text every 0.4 seconds (show/hide toggle)
            bool shouldShow = (int(elapsedSeconds * 2.5f) % 2) == 0; // 2.5 = 1/0.4 for 0.4s blink interval
            if (shouldShow) {
                displayGameMessage("NEW GAME STARTED", 0.0f, 0.0f, 1.0f, 0.0f);
            }
        }
    }

    engine->postRedisplay();
}

/**
 * Logs the current camera position to console
 */
void logCameraPosition() {
    glm::mat4 worldMatrix = camera->getWorldMatrix();
    float x = worldMatrix[3][0];
    float y = worldMatrix[3][1];
    float z = worldMatrix[3][2];
    std::cout << "[CAMERA] Position: (" << std::fixed << std::setprecision(2) 
              << x << ", " << y << ", " << z << ")" << std::endl;
}

/**
 * Handles keyboard input for camera controls and game actions
 */
void keyboardCallback(unsigned char key, int x, int y) {
    // If game is won, block all keys except q, Q, r, R
    if (hanoiGame && hanoiGame->isGameWon()) {
        if (key != 'q' && key != 'Q' && key != 'r' && key != 'R') {
            return;
        }
    }

    float moveSpeed = 1.0f;
    float rotSpeed = 2.0f;

    switch (key) {
    case 'F': case 'f':
        wireframe = !wireframe;
        engine->toggleWireframe(wireframe);
        break;

    case 'H': case 'h':
        showMenu = !showMenu;
        break;

    case 'P': case 'p':
        isRotationMode = !isRotationMode;
        break;

    // Main camera controls
    case '0':
        camera->setLocalMatrix(mainCameraHome);
        isPresetView = false;
        std::cout << "[CAMERA] Main camera activated (free movement mode)" << std::endl;
        break;

    // Camera movement / rotation (WASD)
    case 'w':
    case 'W':
        if (isPresetView) break;
        if (isRotationMode) camera->rotate(rotSpeed, glm::vec3(1, 0, 0));
        else                camera->translate(glm::vec3(0, 0, -moveSpeed));
        if (!isPresetView) {
            logCameraPosition();
            // Prevent camera from leaving scene boundaries
            if (!isRotationMode && hanoiGame) {
                hanoiGame->checkBoundaries();
            }
        }
        break;
    case 's':
    case 'S':
        if (isPresetView) break;
        if (isRotationMode) camera->rotate(-rotSpeed, glm::vec3(1, 0, 0));
        else                camera->translate(glm::vec3(0, 0, moveSpeed));
        if (!isPresetView) {
            logCameraPosition();
            // Prevent camera from leaving scene boundaries
            if (!isRotationMode && hanoiGame) {
                hanoiGame->checkBoundaries();
            }
        }
        break;
    case 'a':
    case 'A':
        if (isPresetView) break;
        if (isRotationMode) {
            // Rotate around world Y axis instead of local Y axis
            glm::mat4 worldMatrix = camera->getWorldMatrix();
            glm::vec3 position = glm::vec3(worldMatrix[3]);
            // Extract rotation part (remove translation)
            worldMatrix[3] = glm::vec4(0, 0, 0, 1);
            // Rotate around world Y axis
            glm::mat4 rotation = glm::rotate(glm::mat4(1.0f), glm::radians(rotSpeed), glm::vec3(0, 1, 0));
            worldMatrix = rotation * worldMatrix;
            // Restore position
            worldMatrix[3] = glm::vec4(position, 1.0f);
            // Convert back to local space
            Node* parent = camera->getParentNode();
            if (parent) {
                glm::mat4 parentWorld = parent->getWorldMatrix();
                glm::mat4 parentInverse = glm::inverse(parentWorld);
                camera->setLocalMatrix(parentInverse * worldMatrix);
            } else {
                camera->setLocalMatrix(worldMatrix);
            }
        }
        else                camera->translate(glm::vec3(-moveSpeed, 0, 0));
        if (!isPresetView) {
            logCameraPosition();
            // Prevent camera from leaving scene boundaries
            if (!isRotationMode && hanoiGame) {
                hanoiGame->checkBoundaries();
            }
        }
        break;
    case 'd':
    case 'D':
        if (isPresetView) break;
        if (isRotationMode) {
            // Rotate around world Y axis instead of local Y axis
            glm::mat4 worldMatrix = camera->getWorldMatrix();
            glm::vec3 position = glm::vec3(worldMatrix[3]);
            // Extract rotation part (remove translation)
            worldMatrix[3] = glm::vec4(0, 0, 0, 1);
            // Rotate around world Y axis
            glm::mat4 rotation = glm::rotate(glm::mat4(1.0f), glm::radians(-rotSpeed), glm::vec3(0, 1, 0));
            worldMatrix = rotation * worldMatrix;
            // Restore position
            worldMatrix[3] = glm::vec4(position, 1.0f);
            // Convert back to local space
            Node* parent = camera->getParentNode();
            if (parent) {
                glm::mat4 parentWorld = parent->getWorldMatrix();
                glm::mat4 parentInverse = glm::inverse(parentWorld);
                camera->setLocalMatrix(parentInverse * worldMatrix);
            } else {
                camera->setLocalMatrix(worldMatrix);
            }
        }
        else                camera->translate(glm::vec3(moveSpeed, 0, 0));
        if (!isPresetView) {
            logCameraPosition();
            // Prevent camera from leaving scene boundaries
            if (!isRotationMode && hanoiGame) {
                hanoiGame->checkBoundaries();
            }
        }
        break;
    /*case 'q':
        if (isPresetView) break;
        if (isRotationMode) camera->rotate(rotSpeed, glm::vec3(0, 0, 1));
        else                camera->translate(glm::vec3(0, moveSpeed, 0));
        break;
    case 'e':
        if (isPresetView) break;
        if (isRotationMode) camera->rotate(-rotSpeed, glm::vec3(0, 0, 1));
        else                camera->translate(glm::vec3(0, -moveSpeed, 0));
        break;*/

    // Fixed camera presets
    case '1': if (hanoiGame) hanoiGame->applyCameraPreset(0); isPresetView = true; break;
    case '2': if (hanoiGame) hanoiGame->applyCameraPreset(1); isPresetView = true; break;

    case 'l': case 'L':
    {
        // Toggle Omni002 light on/off
        if (sceneRoot) {
            Node* omniLightNode = sceneRoot->findRecursiveNodeByName("Omni002");
            if (omniLightNode) {
                Light* omniLight = dynamic_cast<Light*>(omniLightNode);
                if (omniLight) {
                    // Save original values on first toggle
                    if (!omniLightValuesSaved) {
                        omniLightOriginalAmbient = omniLight->getAmbientLightComponent();
                        omniLightOriginalDiffuse = omniLight->getDiffuseLightComponent();
                        omniLightOriginalSpecular = omniLight->getSpecularLightComponent();
                        omniLightValuesSaved = true;
                    }
                    
                    // Toggle light state
                    omniLightEnabled = !omniLightEnabled;
                    
                    // Apply light components based on state
                    if (omniLightEnabled) {
                        // Turn light on by restoring original values
                        omniLight->setAmbientLightComponent(omniLightOriginalAmbient);
                        omniLight->setDiffuseLightComponent(omniLightOriginalDiffuse);
                        omniLight->setSpecularLightComponent(omniLightOriginalSpecular);
                        std::cout << "[LIGHT] Omni002 light enabled" << std::endl;
                    } else {
                        // Turn light off by setting all components to zero
                        omniLight->setAmbientLightComponent(glm::vec4(0.0f));
                        omniLight->setDiffuseLightComponent(glm::vec4(0.0f));
                        omniLight->setSpecularLightComponent(glm::vec4(0.0f));
                        std::cout << "[LIGHT] Omni002 light disabled" << std::endl;
                    }
                }
            }
        }
        break;
    }

    case 'r': case 'R':
    {
        std::cout << "[GAME] Reloading scene..." << std::endl;

        // Clean up old scene
        if (sceneRoot) {
            sceneRoot->removeChildNode(camera);
            delete sceneRoot;
            sceneRoot = nullptr;
        }

        wireframe = false;
        engine->toggleWireframe(wireframe);

        showMenu = true;

        // Reset Omni002 flag (original values will be saved again on next toggle)
        omniLightValuesSaved = false;
        omniLightEnabled = true;

        // Reset camera to initial position
        camera->setLocalMatrix(glm::mat4(1.0f));
        camera->translate(glm::vec3(6.0f, 55.0f, 125.0f));
        camera->rotate(-25.0f, glm::vec3(1.0f, 0.0f, 0.0f));
        mainCameraHome = camera->getLocalMatrix();
        isPresetView = false;

        // Load new scene
        tableNode = ovoReader.readFile("assets/hanoi.ovo", "assets/textures/");

        if (tableNode) {
            sceneRoot = tableNode;
            sceneRoot->addChildNode(camera);

            // Reset game state
            if (hanoiGame) {
                delete hanoiGame;
            }
            // Recreate game instance
            hanoiGame = new Hanoi(camera, engine);
            hanoiGame->initHanoiState(sceneRoot);

            // Show "NEW GAME STARTED" message and save start time
            showNewGameMessage = true;
            newGameMessageStartTime = std::chrono::steady_clock::now();
        }
        else {
            std::cerr << "[ERROR] Failed to reload .ovo file!" << std::endl;
            exit(-1);
        }
        break;
    }

    // Undo/Redo functionality
    case 'u': case 'U':
        if (hanoiGame && !hanoiGame->isGameWon()) {
            hanoiGame->undo();
        }
        break;
    case 'o': case 'O':
        if (hanoiGame && !hanoiGame->isGameWon()) {
            hanoiGame->redo();
        }
        break;
    case 'q':
    case 'Q':
        if (hanoiGame) delete hanoiGame;
        exit(0);
        break;
    }

    engine->postRedisplay();
}

/**
 * Handles window reshape events to update camera aspect ratio
 */
void reshapeCallback(int width, int height) {
    if (height == 0) height = 1;
    PerspectiveCamera* perspectiveCamera = dynamic_cast<PerspectiveCamera*>(camera);
    if (perspectiveCamera) {
        perspectiveCamera->setAspectRatio((float)width, (float)height);
    }
}

/**
 * Recursively prints scene graph structure with node positions
 * @param node Root node to start printing from
 * @param level Current depth level in the tree (for indentation)
 */
void printSceneGraphWithPosition(Node* node, int level = 0) {
    if (!node) return;
    std::string indent(level * 4, ' ');
    std::string branch = (level == 0) ? "ROOT " : "|__ ";
    glm::mat4 worldMatrix = node->getWorldMatrix();
    float x = worldMatrix[3][0];
    float y = worldMatrix[3][1];
    float z = worldMatrix[3][2];

    std::cout << indent << branch << "ID:" << node->getId() << " '" << node->getName() << "'";
    if (dynamic_cast<Mesh*>(node)) std::cout << " [MESH]";
    else if (dynamic_cast<Light*>(node)) std::cout << " [LIGHT]";
    else if (dynamic_cast<Camera*>(node)) std::cout << " [CAMERA]";
    std::cout << std::fixed << std::setprecision(1) << " -> Pos: (" << x << ", " << y << ", " << z << ")" << std::endl;

    for (unsigned int i = 0; i < node->getTotalChildren(); i++) {
        printSceneGraphWithPosition(node->getChildByIndex(i), level + 1);
    }
}

void cleanup() {
    if (hanoiGame) {
        delete hanoiGame;
        hanoiGame = nullptr;
    }

    if (renderList) {
        delete renderList;
        renderList = nullptr;
    }

    if (camera) {
        delete camera;
        camera = nullptr;
    }
}

int main(int argc, char* argv[]) {
    // Initializing the engine
    engine = &Eng::Base::getInstance();
    if (!engine->init(argc, argv)) return -1;

    // Creating the window and enabling FPS
    engine->createMainWindow(800, 600, 100, 100, "Tower of Hanoi - SUPSI");
    engine->enableFPS();

    // Setupping callback functions
    engine->setKeyboardCallback(keyboardCallback);
    engine->setSpecialCallback(specialCallback);
    engine->setDisplayCallback(displayCallback);
    engine->setReshapeCallback(reshapeCallback);

    // Configuring the main camera
    camera = new PerspectiveCamera("MainCam", 45.0f, 800.0f / 600.0f, 1.0f, 5000.0f);
    camera->translate(glm::vec3(6.0f, 55.0f, 125.0f));
    camera->rotate(-25.0f, glm::vec3(1.0f, 0.0f, 0.0f));
    mainCameraHome = camera->getLocalMatrix(); // Save initial position for mobile camera

    // Initializing render list and scene root
    renderList = new RenderList();
    sceneRoot = new Node("Root");

    // Load scene from OVO file
    tableNode = ovoReader.readFile("assets/hanoi.ovo", "assets/textures/");

    if (tableNode) {
        std::cout << "[SCENE] OVO file loaded successfully, adding to scene graph" << std::endl;

        sceneRoot = tableNode;
        sceneRoot->addChildNode(camera);

        printSceneGraphWithPosition(sceneRoot);

        // Initialize game logic
        hanoiGame = new Hanoi(camera, engine);
        hanoiGame->initHanoiState(sceneRoot);

        // Show "NEW GAME STARTED" message at beginning and save start time
        showNewGameMessage = true;
        newGameMessageStartTime = std::chrono::steady_clock::now();

        std::cout << "\n--- SCENE GRAPH STRUCTURE ---" << std::endl;
        printSceneGraphWithPosition(sceneRoot);
        std::cout << "-----------------------------\n" << std::endl;
    }
    else {
        std::cerr << "[ERROR] Critical: Unable to load hanoi.ovo file" << std::endl;
    }

    // Start main loop
    engine->update();

    // Cleanup resources
    cleanup();

    // Freeing the engine
    engine->free();

    return 0;
}