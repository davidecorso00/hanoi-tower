#pragma once

#include <vector>
#include <string>
#include <optional>
#include <iostream>
#include <algorithm>
#include <cmath>
#include <limits>
#include <chrono>
#include <map>

#include "OvoReader.h"
#include "Engine.h"

// Constants
const float MAP_LIMIT_X_L = -90.0f;   // X minimum (left)
const float MAP_LIMIT_X_R = 95.0f;  // X maximum (right)
const float MAP_LIMIT_Y_L = 10.0f;   // Y minimum (down)
const float MAP_LIMIT_Y_R = 70.0f;   // Y maximum (up)
const float MAP_LIMIT_Z_U = -38.0f;  // Z minimum (back)
const float MAP_LIMIT_Z_D = 125.0f;  // Z maximum (forward)

const int KEY_LEFT = 100;
const int KEY_RIGHT = 102;
const int KEY_UP = 101;
const int KEY_DOWN = 103;

/**
 * Represents the state of a disc in the Tower of Hanoi game
 */
struct DiscState {
    Node* node;              // Scene graph node representing the disc
    int sizeRank;            // Size ranking (1 = smallest, 7 = largest)
    glm::mat4 baseTransform; // Base transformation matrix for the disc
};

/**
 * Represents a snapshot of the game state for undo/redo functionality
 */
struct GameState {
    // For each peg, store the size ranks of discs (from bottom to top)
    std::vector<int> pegStacks[3];
    int selectedPeg;
    bool hasHeldDisc;
    int heldDiscSizeRank;
    int heldDiscSourcePeg;
    int moveCount;
    bool isWon;
};

/**
 * Main game logic class for Tower of Hanoi
 * Manages game state, disc movement, win conditions, and camera controls
 */
class Hanoi {
public:
    Hanoi(Camera* cam, Eng::Base* eng);
    ~Hanoi() = default;

    /**
     * Initializes game state by parsing scene graph and setting up discs on pegs
     * @param root Root node of the scene graph containing towers and discs
     */
    void initHanoiState(Node* root);

    /**
     * Updates visual position of held disc with animation effects
     * @param time Time parameter for animation calculations
     */
    void updateHeldDiscVisual(float time);

    /**
     * Handles special key input (arrow keys) for game controls
     * @param key Key code of pressed key
     * @param x Mouse x position (unused)
     * @param y Mouse y position (unused)
     */
    void specialCallback(int key, int x, int y);

    /**
     * Checks and enforces camera boundary constraints
     */
    void checkBoundaries();

    /**
     * Applies a predefined camera preset view
     * @param idx Index of camera preset (0-1)
     */
    void applyCameraPreset(int idx);

    // Public getters
    bool isGameWon() const { return isWon; }
    float getElapsedTime() const; // Returns elapsed game time in seconds
    int getSelectedPeg() const { return selectedPeg; }
    bool hasHeldDisc() const { return heldDisc.has_value(); }
    int getMoveCount() const { return moveCount; }
    
    // Undo/Redo functionality
    void undo();              // Undo last move
    void redo();              // Redo last undone move
    bool canUndo() const { return !undoStack.empty(); }
    bool canRedo() const { return !redoStack.empty(); }

private:
    // External dependencies
    Camera* camera;
    Eng::Base* engine;

    // Game state
    int selectedPeg = 0;                    // Currently selected peg index (0-2)
    bool isWon = false;                     // Victory condition flag
    std::vector<DiscState> pegStacks[3];    // Disc stacks on each of the 3 pegs
    std::optional<DiscState> heldDisc;      // Disc currently held by player
    int heldDiscSourcePeg = -1;             // Peg index from which held disc was taken
    int moveCount = 0;                      // Move counter
    
    // Timer system
    std::chrono::time_point<std::chrono::steady_clock> gameStartTime;
    std::chrono::time_point<std::chrono::steady_clock> gameEndTime;
    bool timerStarted = false;

    // Spatial parameters
    glm::vec3 pegPositions[3];              // World positions of the three pegs
    glm::vec3 cameraTargetCenter;           // Center point for camera targeting
    float baseLevelY = 0.0f;                // Base Y coordinate for disc stacking
    float stackStepY = 0.0f;                // Vertical spacing between stacked discs

    // Selection visualization nodes
    Node* selectionSpot = nullptr;          // Spotlight node for peg selection indicator

    // Camera preset positions
    std::vector<glm::vec3> cameraPresets;
    
    // Undo/Redo system
    std::vector<GameState> undoStack;    // Stack for undo (stores previous states)
    std::vector<GameState> redoStack;    // Stack for redo (stores undone states)

    // Internal logic methods
    void updateSelectedPeg(int delta);              // Updates selected peg index with bounds checking
    int parseDiscSize(const std::string& name);     // Extracts disc size from node name
    void placeDisc(DiscState& disc, int pegIndex, int stackIndex);      // Places disc at specific position
    void buildCameraPresets();                      // Calculates camera preset positions
    void updateSelectionVisuals();                  // Updates visual indicators for selected peg
    void pickupDisc();                              // Picks up top disc from selected peg
    void dropDisc();                                // Drops held disc onto selected peg (with validation)
    bool checkWinCondition();                       // Checks if all discs are on target peg
    GameState saveState() const;                    // Saves current game state
    void restoreState(const GameState& state);      // Restores game state from snapshot
};