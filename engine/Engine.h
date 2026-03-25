/**
 * @file	engine.h
 * @brief	Graphics engine main include file
 *
 * @author	Davide Corso, Marco Soldani, Enrico Vavassori
 */
#pragma once

//////////////
// #INCLUDE //
//////////////

// Main include:
#include "Lib.h"
#include "Camera.h"
#include "RenderList.h"

// C/C++:
#include <iostream>
#include <memory>
#include <vector>
#include <string>
#include <stack>
#include <cstdio>
#include <chrono>
#include <unordered_map>
#include <cassert>
#include <functional>

/////////////
// VERSION //
/////////////

/**
 * @def LIB_NAME
 * @brief Human-readable name of the graphics engine.
 *
 * Used for window titles, logs and debug output.
 */
// Generic info:
#ifdef _DEBUG
   #define LIB_NAME      "CG Project By Marco Soldani, Davide Corso and Enrico Vavassori"   ///< Library credits
#else
    #define LIB_NAME      "CG Project By Marco Soldani, Davide Corso and Enrico Vavassori"   ///< Library credits
#endif

/**
 * @def LIB_VERSION
 * @brief Numeric version of the engine.
 *
 * The actual version is obtained by dividing this value by 10.
 */
#define LIB_VERSION 10


// Engine capability flags <- moved to Lib.h
// #define ENG_RENDER_NORMAL   0x0001

///////////////
// NAMESPACE //
///////////////
/**
* @namespace Eng
* @brief Main namespace containing all engine components
*/
namespace Eng {
    /// Callback type invoked on keyboard input
    using KeyboardCallback = std::function<void(unsigned char key, int x, int y)>;

    /// Callback type invoked during display refresh
    using DisplayCallback = std::function<void()>;

    /// Callback type invoked on special key input
    using SpecialCallback = std::function<void(int key, int x, int y)>;

    /// Callback type invoked when the window is resized
    using ReshapeCallback = std::function<void(int width, int height)>;

    /**
     * @brief Core engine singleton class.
     *
     * This class represents the main entry point of the graphics engine.
     * It is responsible for:
     * - initializing and shutting down the engine
     * - managing the rendering loop
     * - handling window creation and events
     * - dispatching input callbacks
     * - coordinating camera and render lists
     */
    class ENG_API Base final
    {
    public:    
        /// Deleted copy constructor (singleton)
        Base(Base const&) = delete;

        /// Deleted copy assignment operator (singleton)
        void operator=(Base const&) = delete;

        /**
         * @brief Returns the singleton instance of the engine.
         * @return Reference to the unique Base instance.
         */
        static Base& getInstance();
        
        /**
         * @brief Initializes the engine.
         * @param argc Command-line argument count.
         * @param argv Command-line argument values.
         * @return True if initialization succeeded.
         */
        bool init(int argc, char* argv[]);
        
        /**
         * @brief Releases all engine resources.
         * @return True if shutdown succeeded.
         */
        bool free();

        /**
         * @brief Sets the main render list.
         * @param list Pointer to the render list.
         */
        void setRenderList(RenderList* list);
        
        /**
         * @brief Sets the main camera used for rendering.
         * @param camera Pointer to the camera.
         */
        void setMainCamera(Camera* camera);
        
        /**
         * @brief Creates the main application window.
         */
        void createMainWindow(int width, int height, int x, int y, const char* title);
        
        /**
         * @brief Starts the engine update loop.
         */
        void update();

        /**
         * @brief Renders the current frame.
         */
        void render();

        /**
         * @brief Registers the display callback.
         */
        void setDisplayCallback(DisplayCallback callback);
        
        /**
         * @brief Registers the reshape callback.
         */
        void setReshapeCallback(ReshapeCallback callback);
        
        /**
         * @brief Registers the keyboard callback.
         */
        void setKeyboardCallback(KeyboardCallback callback);
        
        /**
         * @brief Registers the special key callback.
         */
        void setSpecialCallback(SpecialCallback callback);

        /**
         * @brief Sets the OpenGL clear color.
         */
        void setClearColor(float r, float g, float b, float a);
        
        /**
         * @brief Enables or disables lighting.
         */
        void setLighting(bool enable);
       
        
        /**
         * @brief Enables or disables texture mapping.
         */
        void setTexture(bool enable);

        /**
         * @brief Handles a display refresh request.
         */
        void handleDisplayRequest();

        /**
         * @brief Handles a window resize event.
         */
        void handleReshapeRequest(int width, int height);
        
        /**
         * @brief Handles a keyboard input event.
         */
        void handleKeyboardRequest(unsigned char key, int x, int y);
        
        /**
         * @brief Handles a special key input event.
         */
        void handleSpecialRequest(int key, int x, int y);


        /**
         * @brief Enables FPS.
         */
        void enableFPS();

        /**
         * @brief Disables FPS computation and display.
         */
        void disableFPS();

        /**
         * @brief Sets the game timer value in seconds.
         */
        void setGameTimer(float time);

        /**
         * @brief Sets the move counter value.
         */
        void setMoveCount(int count);

        /**
         * @brief Updates the FPS counter.
         */
        void calculateFPS();

        /**
         * @brief Adds a string to the on-screen text buffer.
         */
        void addToScreenText(std::string text);
        
        /**
         * @brief Clears all on-screen text.
         */
        void clearScreenText();

        /**
         * @brief Requests a screen redraw.
         */
        void postRedisplay();

        /**
         * @brief Draws a colored string at the given screen coordinates.
         */
        void addString(float x, float y, std::string text, float r, float g, float b);
        
        /**
         * @brief Returns the window width.
         */
        int getWindowWidth();

        /**
         * @brief Returns the window height.
         */
        int getWindowHeight();

        /**
         * @brief Returns the pixel width of the given text string.
         */
        int getTextWidth(const std::string& text);

        void toggleWireframe(const bool state);
    private:
        /** @brief Reserved implementation details */
        struct Reserved;
        std::unique_ptr<Reserved> reserved;

        // Const/dest:
        Base();
        ~Base();

        // Fps info
        bool isFpsEnabled;
        int numFrame = 0;
        float fps = 0.0f;

        std::chrono::time_point<std::chrono::steady_clock> lastFpsUpdateTime;
    };
}; // end of namespace Eng::