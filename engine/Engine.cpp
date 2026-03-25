/**
 * @file	engine.cpp
 * @brief	Graphics engine main file
 *
 * @author
   Davide Corso,
   Marco Soldani,
   Enrico Vavassori
 */

//////////////
// #INCLUDE //
//////////////

// Main include:
#include "Engine.h"
#include "PerspectiveCamera.h"
#include "OrthographicCamera.h"

#ifdef _WINDOWS
#include <Windows.h>
#endif

#include <FreeImage.h>
   
// C/C++:
#include <iostream>   
#include <source_location>
#include <GL/freeglut.h>
#include <glm/gtc/type_ptr.hpp>


/////////////////////////
// RESERVED STRUCTURES //
/////////////////////////

/**
 * @brief Deferred text rendering request.
 *
 * This structure represents a request to render text on screen.
 * Text is not rendered immediately, but collected during the frame
 * and rendered in a dedicated 2D overlay pass.
 */
struct TextRequest {
    std::string message;
    glm::vec2 position;    ///< Screen position
    glm::vec3 color;       ///< RGB color
};

/**
 * @brief Base class reserved structure.
 */
 // ======================================================
 // Internal PIMPL structure
 // ======================================================
struct Eng::Base::Reserved
{
    bool initFlag = false;  ///< Indicates whether the engine is initialized

    int windowWidth = 800;  ///< Current window width
    int windowHeight = 600;  ///< Current window height

    RenderList* currentList = nullptr;  ///< Active render list
    Camera* currentCamera = nullptr;  ///< Active camera

    std::unique_ptr<OrthographicCamera> uiCamera;

    bool isFpsEnabled = false;  ///< Enables FPS computation and display
    int numFrame = 0;  ///< Frame counter for FPS calculation
    float fps = 0.0f;  ///< Current FPS value

    /// Timestamp of the last FPS update
    std::chrono::time_point<std::chrono::steady_clock> lastFpsUpdateTime = std::chrono::steady_clock::now();

    float gameTimer = 0.0f;  ///< Game timer in seconds
    int moveCount = 0;  ///< Move counter

    std::vector<std::string> screenText;  ///< Generic on-screen text lines
    std::vector<TextRequest> victoryMessage;  ///< Deferred victory text requests

    Eng::KeyboardCallback clientKeyboardCb = nullptr;  ///< Client keyboard callback
    Eng::DisplayCallback clientDisplayCb = nullptr;  ///< Client display callback
    Eng::SpecialCallback clientSpecialCb = nullptr;  ///< Client special-key callback
    Eng::ReshapeCallback clientReshapeCb = nullptr;  ///< Client reshape callback

    Reserved() {
        uiCamera = std::make_unique<OrthographicCamera>("UI_Cam", 0.0f, 800.0f, 0.0f, 600.0f, -1.0f, 1.0f);
    }
};

//////////////////////
// GLUT C WRAPPERS  //
//////////////////////
/**
 * @brief Helper function returning the engine singleton.
 *
 * Used by GLUT wrapper functions to forward C callbacks
 * to the C++ engine instance.
 */
static Eng::Base& engine() {
    return Eng::Base::getInstance();
}

/**
 * @brief GLUT display callback wrapper.
 *
 * Bridges the C-style GLUT display callback to the engine's
 * object-oriented display handler.
 */
static void glutDisplayWrapper() {
    engine().handleDisplayRequest();
}

/**
 * @brief GLUT reshape callback wrapper.
 */
static void glutReshapeWrapper(int width, int height) {
    engine().handleReshapeRequest(width, height);
}

/**
 * @brief GLUT keyboard callback wrapper.
 */
static void glutKeyboardWrapper(unsigned char key, int x, int y) {
    engine().handleKeyboardRequest(key, x, y);
}

/**
 * @brief GLUT special key callback wrapper.
 */
static void glutSpecialWrapper(int key, int x, int y) {
    engine().handleSpecialRequest(key, x, y);
}


////////////////////////
// BODY OF CLASS Base //
////////////////////////

/**
 * @brief Engine constructor.
 *
 * Allocates and initializes the internal reserved structure.
 */
Eng::Base::Base() : reserved(std::make_unique<Eng::Base::Reserved>())
{
}

/**
 * @brief Engine destructor.
 *
 * Automatically releases all engine resources.
 */
Eng::Base::~Base() {
    free();
}

/**
 * @brief Returns the unique engine singleton instance.
 *
 * @return Reference to the engine instance
 */
Eng::Base &Eng::Base::getInstance() {
   static Base instance;
   return instance;
}

/**
 * @brief Initializes the engine and its dependencies.
 *
 * Initializes GLUT, FreeImage and default engine state.
 *
 * @param argc Command-line argument count
 * @param argv Command-line argument values
 * @return True if initialization succeeds
 */
bool Eng::Base::init(int argc, char* argv[]) {
   // Already initialized?
   if (reserved->initFlag) {
      std::cout << "ERROR: engine already initialized" << std::endl;
      return false;
   }

   // Here you can initialize most of the graphics engine's dependencies and default settings
   glutInit(&argc, argv);
   FreeImage_Initialise();
   
   // Done:
   std::cout << "[>] " << LIB_NAME << " initialized" << std::endl;
   reserved->initFlag = true;
   return true;
}

/**
 * @brief Free internal components.
 * @return TF
 */
bool Eng::Base::free() {
   // Not initialized?
   if (!reserved->initFlag) {
      std::cout << "ERROR: engine not initialized" << std::endl;
      return false;
   }

   // Here you can properly dispose of any allocated resource (including third-party dependencies)
   reserved->initFlag = false;

   // Done:
   std::cout << "[<] " << LIB_NAME << " deinitialized" << std::endl;
   reserved->initFlag = false;
   return true;
}

/**
 * @brief Creates the main application window and initializes OpenGL state.
 *
 * Sets up the GLUT window, registers callbacks and configures
 * the OpenGL fixed-function pipeline.
 */
void Eng::Base::createMainWindow(int width, int height, int x, int y, const char* title) {
    // Not initialized?
    if (!reserved->initFlag) {
        return;
    }

    glutInitDisplayMode(GLUT_RGB | GLUT_DOUBLE | GLUT_DEPTH);
    glutInitWindowSize(width, height);
    glutInitWindowPosition(x, y);
    glutCreateWindow(title);

    reserved->uiCamera->setFrustum(0.0f, (float)width, 0.0f, (float)height, -1.0f, -1.0f);

    glutDisplayFunc(glutDisplayWrapper);
    glutReshapeFunc(glutReshapeWrapper);
    glutKeyboardFunc(glutKeyboardWrapper);
    glutSpecialFunc(glutSpecialWrapper);

    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);
    glEnable(GL_LIGHTING);
    glShadeModel(GL_SMOOTH);
    glEnable(GL_NORMALIZE);
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
}

/**
 * @brief Starts the main rendering loop.
 */
void Eng::Base::update() {
    glutMainLoop();
}

void Eng::Base::setDisplayCallback(DisplayCallback callback) {
    reserved->clientDisplayCb = callback;
}

void Eng::Base::setReshapeCallback(ReshapeCallback callback) {
    reserved->clientReshapeCb = callback;
}

void Eng::Base::setKeyboardCallback(KeyboardCallback callback) {
    reserved->clientKeyboardCb = callback;
}

void Eng::Base::setSpecialCallback(SpecialCallback cb) {
    reserved->clientSpecialCb = cb;
}

void Eng::Base::setClearColor(float r, float g, float b, float a) {
    glClearColor(r, g, b, a);
}

void Eng::Base::setLighting(bool enable) {
    if (enable) glEnable(GL_LIGHTING);
    else glDisable(GL_LIGHTING);
}

void Eng::Base::setTexture(bool enable) {
    if (enable) glEnable(GL_TEXTURE_2D);
    else glDisable(GL_TEXTURE_2D);
}

void Eng::Base::setRenderList(RenderList* list) {
    reserved->currentList = list;
}

void Eng::Base::setMainCamera(Camera* camera) {
    reserved->currentCamera = camera;
}

void Eng::Base::render() {
    if (!reserved->currentCamera || !reserved->currentList) {
        return;
    }

    glEnable(GL_DEPTH_TEST);
    glEnable(GL_LIGHTING);
    glEnable(GL_TEXTURE_2D);

    glColor4f(1.0f, 1.0f, 1.0f, 1.0f);

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glMatrixMode(GL_PROJECTION);
    glLoadMatrixf(glm::value_ptr(reserved->currentCamera->getCameraProjectionMatrix()));

    glm::mat4 viewMatrix = reserved->currentCamera->getInverseCameraMatrix();

    reserved->currentList->render(viewMatrix);

    calculateFPS();

    glDisable(GL_LIGHTING);
    glDisable(GL_TEXTURE_2D);
    glDisable(GL_DEPTH_TEST);

    glMatrixMode(GL_PROJECTION);
    glLoadMatrixf(glm::value_ptr(reserved->uiCamera->getCameraProjectionMatrix()));

    glMatrixMode(GL_MODELVIEW);
    glLoadMatrixf(glm::value_ptr(glm::mat4(1.0f)));

    // FPS
    if (reserved->isFpsEnabled) {
        glColor4f(0.0f, 1.0f, 0.0f, 1.0f);
        char buffer[64];
        snprintf(buffer, sizeof(buffer), "FPS: %.2f", reserved->fps);
        glRasterPos2f(reserved->windowWidth - 100.0f, reserved->windowHeight - 12.0f);
        glutBitmapString(GLUT_BITMAP_8_BY_13, (unsigned char*)buffer);
    }

    // Timer
    if (reserved->gameTimer > 0.0f) {
        glColor4f(0.0f, 1.0f, 0.0f, 1.0f);
        char buffer[64];
        int seconds = static_cast<int>(reserved->gameTimer);
        snprintf(buffer, sizeof(buffer), "Time: %ds", seconds);
        glRasterPos2f(reserved->windowWidth - 100.0f, reserved->windowHeight - 24.0f);
        glutBitmapString(GLUT_BITMAP_8_BY_13, (unsigned char*)buffer);
    }

    // Move counter
    if (reserved->moveCount >= 0) {
        glColor4f(0.0f, 1.0f, 0.0f, 1.0f);
        char buffer[64];
        snprintf(buffer, sizeof(buffer), "Moves: %d", reserved->moveCount);
        glRasterPos2f(reserved->windowWidth - 100.0f, reserved->windowHeight - 36.0f);
        glutBitmapString(GLUT_BITMAP_8_BY_13, (unsigned char*)buffer);
    }

    // Menu
    float textYPosition = reserved->windowHeight - 12.0f;
    glColor4f(1.0f, 1.0f, 1.0f, 1.0f);
    for (const auto& toPrint : reserved->screenText) {
        glRasterPos2f(0.0f, textYPosition);
        glutBitmapString(GLUT_BITMAP_8_BY_13, (unsigned char*)toPrint.c_str());
        textYPosition -= 12;
    }

    // Victory
    void* victoryMessageFont = GLUT_BITMAP_HELVETICA_18;
    for (const auto& item : reserved->victoryMessage) {
        glColor3f(item.color[0], item.color[1], item.color[2]);
        glRasterPos2f(item.position[0], item.position[1]);
        glutBitmapString(victoryMessageFont, (const unsigned char*)item.message.c_str());
    }

    glEnable(GL_LIGHTING);
    glutSwapBuffers();
}

void Eng::Base::handleDisplayRequest() {
    if (reserved->clientDisplayCb) {
        reserved->clientDisplayCb();
    }
    this->render();
}

void Eng::Base::handleReshapeRequest(int width, int height) {
    if (height == 0) {
        height = 1;
    }
    reserved->windowWidth = width;
    reserved->windowHeight = height;
    glViewport(0, 0, width, height);

    reserved->uiCamera->setFrustum(0.0f, (float)width, 0.0f, (float)height, -1.0f, 1.0f);

    if (reserved->clientReshapeCb) {
        reserved->clientReshapeCb(width, height);
    }
}

void Eng::Base::handleKeyboardRequest(unsigned char key, int x, int y) {
    if (reserved->clientKeyboardCb) {
        reserved->clientKeyboardCb(key, x, y);
    }
}

void Eng::Base::handleSpecialRequest(int key, int x, int y) {
    if (reserved->clientSpecialCb) {
        reserved->clientSpecialCb(key, x, y);
    }
}

void ENG_API Eng::Base::calculateFPS() {
    reserved->numFrame++;

    auto currentTime = std::chrono::steady_clock::now();
    std::chrono::duration<float> elapsedTime = currentTime - reserved->lastFpsUpdateTime;

    if (elapsedTime.count() >= 1.0f) {
        reserved->fps = reserved->numFrame / elapsedTime.count();
        reserved->numFrame = 0;
        reserved->lastFpsUpdateTime = currentTime;
    }
}

void ENG_API Eng::Base::enableFPS() {
    reserved->isFpsEnabled = true;
}

void ENG_API Eng::Base::disableFPS() {
    reserved->isFpsEnabled = false;
}

void ENG_API Eng::Base::setGameTimer(float time) {
    reserved->gameTimer = time;
}

void ENG_API Eng::Base::setMoveCount(int count) {
    reserved->moveCount = count;
}

void Eng::Base::postRedisplay() {
    glutPostRedisplay();
}

void Eng::Base::addToScreenText(std::string text) {
    reserved->screenText.push_back(text);
}

void Eng::Base::addString(float x, float y, std::string text, float r, float g, float b) {
    TextRequest req;

    req.message = text;
    req.position = glm::vec2(x, y);
    req.color = glm::vec3(r, g, b);

    reserved->victoryMessage.push_back(req);
}

void Eng::Base::clearScreenText() {
    reserved->screenText.clear();
    reserved->victoryMessage.clear();
}

int Eng::Base::getWindowWidth() { return reserved->windowWidth; }
int Eng::Base::getWindowHeight() { return reserved->windowHeight; }

int Eng::Base::getTextWidth(const std::string& text) {
    return glutBitmapLength(GLUT_BITMAP_TIMES_ROMAN_24, (const unsigned char*)text.c_str());
}

void Eng::Base::toggleWireframe(bool state) {
    if (state) { glPolygonMode(GL_FRONT_AND_BACK, GL_LINE); }
    else { glPolygonMode(GL_FRONT_AND_BACK, GL_FILL); }
}