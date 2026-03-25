#include "Hanoi.h"
#include <map>

Hanoi::Hanoi(Camera* cam, Eng::Base* eng)
    : camera(cam), engine(eng)
{
    cameraPresets.resize(2);
}

/**
 * Updates the selected peg index, clamping to valid range [0, 2]
 * @param delta Change in peg index (+1 for right, -1 for left)
 */
void Hanoi::updateSelectedPeg(int delta) {
    int newPegIndex = selectedPeg + delta;
    // Clamp to valid range [0, 2] without wrap-around
    if (newPegIndex < 0) newPegIndex = 0;
    if (newPegIndex > 2) newPegIndex = 2;
    
    if (newPegIndex != selectedPeg) {
        selectedPeg = newPegIndex;
        std::cout << "[Hanoi] Selected peg changed to: " << selectedPeg 
                  << " (Z=" << pegPositions[selectedPeg].z << ")" << std::endl;
        updateSelectionVisuals();
    }
}

/**
 * Parses disc size from node name (e.g., "Disk_1" -> 1, "Disk_7" -> 7)
 * @param name Node name string
 * @return Size rank (1-7) or 0 if invalid
 */
int Hanoi::parseDiscSize(const std::string& name) {
    // Discs are named "Disk_1", "Disk_2", ..., "Disk_7"
    if (name.rfind("Disk_", 0) != 0) {
        return 0;
    }
    
    int size = std::atoi(name.c_str() + 5); // "Disk_" = 5 characters
    if (size <= 0 || size > 7) return 0;
    // Note: Disk_1 -> 1 (smallest), Disk_7 -> 7 (largest)
    // Higher sizeRank = larger disc
    return size;
}

/**
 * Places a disc at a specific position on a peg stack
 * @param disc Disc state to place
 * @param pegIndex Target peg index (0-2)
 * @param stackIndex Position in stack (0 = bottom)
 */
void Hanoi::placeDisc(DiscState& disc, int pegIndex, int stackIndex) {
    glm::vec3 position = pegPositions[pegIndex];
    position.y = baseLevelY + stackStepY * static_cast<float>(stackIndex);
    glm::mat4 transform = disc.baseTransform;
    transform[3] = glm::vec4(position, 1.0f);
    disc.node->setLocalMatrix(transform);
}

/**
 * Updates visual position of held disc with hover
 * @param time Time parameter for sine wave animation
 */
void Hanoi::updateHeldDiscVisual(float time) {
    if (!heldDisc.has_value()) {
        return;
    }

    auto& stack = pegStacks[selectedPeg];
    int topIndex = static_cast<int>(stack.size());
    glm::vec3 position = pegPositions[selectedPeg];
    float lift = 5.0f;  // Height offset above stack
    
    position.y = baseLevelY + stackStepY * static_cast<float>(topIndex) + lift;
    glm::mat4 transform = heldDisc->baseTransform;
    transform[3] = glm::vec4(position, 1.0f);
    heldDisc->node->setLocalMatrix(transform);
}

/**
 * Builds camera preset positions with fixed coordinates
 * Creates 2 different viewing angles
 */
void Hanoi::buildCameraPresets() {
    // Fixed camera preset positions
    cameraPresets[0] = glm::vec3(5.0f, 69.0f, 80.0f);   // Preset 0
    cameraPresets[1] = glm::vec3(48.0f, 63.0f, 77.0f);  // Preset 1
}

/**
 * Applies a camera preset view by setting camera position and orientation
 * @param idx Preset index (0-1)
 */
void Hanoi::applyCameraPreset(int idx) {
    if (idx < 0 || idx >= (int)cameraPresets.size()) return;
    glm::vec3 position = cameraPresets[idx];
    glm::vec3 up = glm::vec3(0, 1, 0);
    glm::mat4 viewMatrix = glm::lookAt(position, cameraTargetCenter, up);
    glm::mat4 worldMatrix = glm::inverse(viewMatrix);
    camera->setLocalMatrix(worldMatrix);
}

/**
 * Updates visual indicators (spotlight) to show currently selected peg
 */
void Hanoi::updateSelectionVisuals() {
    // Safety check
    if (selectedPeg < 0 || selectedPeg > 2) return;

    // Get X and Z position of current peg
    glm::vec3 pegPosition = pegPositions[selectedPeg];

    // Move spotlight (Spot001) to selected peg position
    if (selectionSpot) {
        // Get current world position to preserve Y height
        glm::mat4 worldMatrix = selectionSpot->getWorldMatrix();
        glm::vec3 currentWorldPos = glm::vec3(worldMatrix[3]);
        
        // Calculate new world position: keep Y, update X and Z to match peg
        glm::vec3 newWorldPos = glm::vec3(pegPosition.x, currentWorldPos.y, pegPosition.z);
        
        // Get parent's world matrix to convert to local space
        Node* parent = selectionSpot->getParentNode();
        glm::mat4 parentWorldMatrix = parent ? parent->getWorldMatrix() : glm::mat4(1.0f);
        glm::mat4 parentInverse = glm::inverse(parentWorldMatrix);
        
        // Convert new world position to local space
        glm::vec4 newWorldPos4 = glm::vec4(newWorldPos, 1.0f);
        glm::vec4 newLocalPos4 = parentInverse * newWorldPos4;
        
        // Update local matrix: preserve rotation, update only translation
        glm::mat4 localTransform = selectionSpot->getLocalMatrix();
        localTransform[3] = newLocalPos4;
        selectionSpot->setLocalMatrix(localTransform);
    }
}

/**
 * Initializes game state by parsing scene graph and setting up initial disc positions
 * @param root Root node of scene graph containing towers and discs
 */
void Hanoi::initHanoiState(Node* root) {
    // Clear state variables (important for scene reset [R] key)
    for (int i = 0; i < 3; i++) {
        pegStacks[i].clear();
    }
    heldDisc.reset();
    heldDiscSourcePeg = -1;
    isWon = false;        // Reset victory state
    timerStarted = false; // Reset timer
    moveCount = 0;        // Reset move counter
    
    // Clear undo/redo stacks
    undoStack.clear();
    redoStack.clear();

    // Find tower nodes in scene graph
    Node* tower1 = root->findRecursiveNodeByName("Tower_1");
    Node* tower2 = root->findRecursiveNodeByName("Tower_2");
    Node* tower3 = root->findRecursiveNodeByName("Tower_3");
    if (!tower1 || !tower2 || !tower3) {
        std::cerr << "[Hanoi] ERROR: Unable to find tower pegs (Tower_1/2/3)" << std::endl;
        return;
    }
    
    // Extract peg world positions
    pegPositions[0] = glm::vec3(tower1->getWorldMatrix()[3]);
    pegPositions[1] = glm::vec3(tower2->getWorldMatrix()[3]);
    pegPositions[2] = glm::vec3(tower3->getWorldMatrix()[3]);
    cameraTargetCenter = (pegPositions[0] + pegPositions[1] + pegPositions[2]) / 3.0f;

    // Find all discs (Disk_1, Disk_2, ..., Disk_7)
    std::vector<DiscState> discs;
    for (int i = 1; i <= 7; ++i) {
        std::string name = "Disk_" + std::to_string(i);
        Node* discNode = root->findRecursiveNodeByName(name);
        if (!discNode) {
            std::cerr << "[Hanoi] WARNING: Disc " << i << " not found (Disk_" << i << ")" << std::endl;
            continue;
        }
        DiscState discState{ discNode, parseDiscSize(name), discNode->getLocalMatrix() };
        discs.push_back(discState);
        std::cout << "[Hanoi] Disc loaded: " << name << " (size rank: " << discState.sizeRank << ")" << std::endl;
    }
    
    if (discs.empty()) {
        std::cerr << "[Hanoi] ERROR: No discs found in scene!" << std::endl;
    } else {
        std::cout << "[Hanoi] Successfully loaded " << discs.size() << " discs" << std::endl;
    }

    // Assign discs to the nearest peg based on distance in the XZ plane
    // By retrieving the world coord of a disk you can assume in which peg the disk is
    for (auto& disc : discs) {

        // Extract the world-space position of the disc from its transformation matrix
        glm::vec3 position = glm::vec3(disc.node->getWorldMatrix()[3]);

        // Initialize the best (minimum) distance found so far
        float bestDistance = std::numeric_limits<float>::max();

        // Index of the peg currently closest to the disc
        int bestPegIndex = 0;

        // Iterate over all pegs (three in the Tower of Hanoi)
        for (int pegIndex = 0; pegIndex < 3; ++pegIndex) {

            // Disc position projected onto the XZ plane
            glm::vec2 discPos{ position.x, position.z };

            // Peg position projected onto the XZ plane
            glm::vec2 pegPos{ pegPositions[pegIndex].x, pegPositions[pegIndex].z };

            // Compute squared distance between disc and peg in the XZ plane
            // (squared distance avoids the cost of a square root)
            float distanceSquared =
                glm::dot(discPos - pegPos, discPos - pegPos);

            // Update the closest peg if a shorter distance is found
            if (distanceSquared < bestDistance) {
                bestDistance = distanceSquared;   // Store the new minimum distance
                bestPegIndex = pegIndex;          // Store the index of the closest peg
            }
        }

        // Assign the disc to the stack corresponding to the closest peg
        pegStacks[bestPegIndex].push_back(disc);
    }


    // Sort each peg stack by Y coordinate (bottom to top)
    // Used to sort each peg in order to find out which one is on the top -> useful when you pick the top disk
    for (int pegIndex = 0; pegIndex < 3; ++pegIndex) {
        std::sort(pegStacks[pegIndex].begin(), pegStacks[pegIndex].end(),
            [](const DiscState& discA, const DiscState& discB) {
                float yA = glm::vec3(discA.node->getWorldMatrix()[3]).y;
                float yB = glm::vec3(discB.node->getWorldMatrix()[3]).y;
                return yA < yB;
            });
    }

    // Calculate base level and step height using the most populated peg
    int maxPegIndex = 0;

    // Return the peg with the higher number of disks
    for (int pegIndex = 1; pegIndex < 3; ++pegIndex) {
        if (pegStacks[pegIndex].size() > pegStacks[maxPegIndex].size()) maxPegIndex = pegIndex;
    }

    if (pegStacks[maxPegIndex].size() > 1) {
        float minY = glm::vec3(pegStacks[maxPegIndex].front().node->getWorldMatrix()[3]).y;  // Bottom disk
        float maxY = glm::vec3(pegStacks[maxPegIndex].back().node->getWorldMatrix()[3]).y;   // Upper disk
        stackStepY = (maxY - minY) / float(pegStacks[maxPegIndex].size() - 1);  // Distance between disks
        baseLevelY = minY;  // Save the position where you will put the next disk
    }

    // Default step height if only one disc
    else if (!pegStacks[maxPegIndex].empty()) {
        baseLevelY = glm::vec3(pegStacks[maxPegIndex].front().node->getWorldMatrix()[3]).y;
        stackStepY = 0.7f;  
    }

    // Realign all discs to their stack positions
    for (int pegIndex = 0; pegIndex < 3; ++pegIndex) {
        for (int stackIndex = 0; stackIndex < (int)pegStacks[pegIndex].size(); ++stackIndex) {
            placeDisc(pegStacks[pegIndex][stackIndex], pegIndex, stackIndex);
        }
    }

    // Build camera preset positions
    buildCameraPresets();

    // Set initial selected peg to the one with most discs
    selectedPeg = maxPegIndex;
    heldDisc.reset();

    // Find selection spotlight node
    selectionSpot = root->findRecursiveNodeByName("Spot001");

    // Update initial selection visuals
    updateSelectionVisuals();
    
    // Start timer when game begins
    gameStartTime = std::chrono::steady_clock::now();
    timerStarted = true;
}

/**
 * Picks up the top disc from the currently selected peg
 */
void Hanoi::pickupDisc() {
    if (heldDisc.has_value()) return; // Already holding a disc
    auto& stack = pegStacks[selectedPeg];
    if (stack.empty()) return;        // No disc to pick up
    
    // Save state before picking up disc (for undo)
    undoStack.push_back(saveState());

    // Clear redo stack when making a new action
    redoStack.clear();
    
    heldDiscSourcePeg = selectedPeg;
    heldDisc = stack.back();
    stack.pop_back();
}

/**
 * Drops the held disc onto the currently selected peg
 * Validates that disc placement follows Tower of Hanoi rules
 */
void Hanoi::dropDisc() {
    if (!heldDisc.has_value()) return;

    auto& stack = pegStacks[selectedPeg];
    // Rule: Cannot place a larger disc on top of a smaller disc
    // Rule: Can only place a disc if it's smaller than the top disc (or stack is empty)
    // Note: Higher sizeRank = larger disc
    // If stack is not empty, held disc must be smaller (lower sizeRank) than top disc
    if (!stack.empty()) {
        int heldDiscSize = heldDisc->sizeRank;
        int topDiscSize = stack.back().sizeRank;
        std::cout << "[Hanoi] Attempting to place disc (size=" << heldDiscSize 
                  << ") on top of stack (top disc size=" << topDiscSize << ")" << std::endl;
        
        // Held disc must be smaller (lower sizeRank) than top disc
        // Higher sizeRank = larger disc
        if (heldDiscSize >= topDiscSize) {
            std::cout << "[Hanoi] BLOCKED: Cannot place larger or equal disc (size=" << heldDiscSize 
                      << ") on top of smaller disc (size=" << topDiscSize << ")" << std::endl;
            return;
        }
        std::cout << "[Hanoi] SUCCESS: Placed smaller disc (size=" << heldDiscSize 
                  << ") on top of larger disc (size=" << topDiscSize << ")" << std::endl;
    }
    
    // Check if this is an actual move (disc moved to different peg)
    bool isActualMove = (heldDiscSourcePeg != selectedPeg);
    
    // Save state BEFORE placing the disc (for undo)
    // This way, when we undo, we go back to the state with disc in hand
    if (isActualMove) {
        undoStack.push_back(saveState());
        // Clear redo stack when making a new move (can't redo after a new action)
        redoStack.clear();
    }
    
    int stackIndex = static_cast<int>(stack.size());
    stack.push_back(*heldDisc);
    placeDisc(stack.back(), selectedPeg, stackIndex);

    // Increment move counter only if disc was actually moved to a different peg
    if (isActualMove) {
        moveCount++;
    }

    heldDisc.reset();
    heldDiscSourcePeg = -1;

    // Check win condition after placing disc
    if (checkWinCondition()) {
        isWon = true;
        // Stop timer when game is won
        if (timerStarted) {
            gameEndTime = std::chrono::steady_clock::now();
        }
        std::cout << "[GAME] Victory! All discs successfully moved!" << std::endl;
    }
}

/**
 * Handles special key input for game controls
 * @param key Key code (arrow keys)
 * @param x Mouse x position (unused)
 * @param y Mouse y position (unused)
 */
void Hanoi::specialCallback(int key, int x, int y) {
    if (isWon) return; // Block game input if won

    std::cout << "[Hanoi] Special key pressed: " << key << " (LEFT=" << KEY_LEFT 
              << ", UP=" << KEY_UP << ", RIGHT=" << KEY_RIGHT << ", DOWN=" << KEY_DOWN << ")" << std::endl;

    switch (key) {
    case KEY_LEFT:  
        std::cout << "[Hanoi] LEFT arrow - switching to next peg" << std::endl;
        updateSelectedPeg(+1); 
        break;
    case KEY_RIGHT: 
        std::cout << "[Hanoi] RIGHT arrow - switching to previous peg" << std::endl;
        updateSelectedPeg(-1); 
        break;
    case KEY_UP:    
        std::cout << "[Hanoi] UP arrow - picking up disc from selected peg" << std::endl;
        pickupDisc(); 
        break;
    case KEY_DOWN:  
        std::cout << "[Hanoi] DOWN arrow - dropping disc onto selected peg" << std::endl;
        dropDisc(); 
        break;
    default: 
        std::cout << "[Hanoi] Unrecognized key code: " << key << std::endl;
        return;
    }
}

/**
 * Checks camera position against scene boundaries and clamps if necessary
 * Prevents camera from moving outside the playable area
 */
void Hanoi::checkBoundaries() {
    if (!camera) return;

    glm::mat4 transform = camera->getLocalMatrix();
    glm::vec3 position = glm::vec3(transform[3]);
    bool hasCollision = false;

    // Check X boundaries
    if (position.x > MAP_LIMIT_X_R) {
        position.x = MAP_LIMIT_X_R;
        hasCollision = true;
    }
    else if (position.x < MAP_LIMIT_X_L) {
        position.x = MAP_LIMIT_X_L;
        hasCollision = true;
    }

    // Check Y boundaries
    if (position.y > MAP_LIMIT_Y_R) {
        position.y = MAP_LIMIT_Y_R;
        hasCollision = true;
    }
    else if (position.y < MAP_LIMIT_Y_L) {
        position.y = MAP_LIMIT_Y_L;
        hasCollision = true;
    }

    // Check Z boundaries
    if (position.z > MAP_LIMIT_Z_D) {
        position.z = MAP_LIMIT_Z_D;
        hasCollision = true;
    }
    else if (position.z < MAP_LIMIT_Z_U) {
        position.z = MAP_LIMIT_Z_U;
        hasCollision = true;
    }

    // Apply clamped position if collision detected
    if (hasCollision) {
        transform[3][0] = position.x; transform[3][1] = position.y; transform[3][2] = position.z;
        camera->setLocalMatrix(transform);
    }
}

/**
 * Checks if win condition is met (all discs on target peg)
 * Tower of Hanoi: Player wins by moving all discs to Tower_2 (index 1) OR Tower_3 (index 2)
 * The discs start on Tower_1 (index 0) and must be moved to a different peg
 * @return true if game is won, false otherwise
 */
bool Hanoi::checkWinCondition() {
    int totalDiscs = pegStacks[0].size() + pegStacks[1].size() + pegStacks[2].size();

    // Include held disc in total count
    if (heldDisc.has_value()) {
        totalDiscs++;
    }
    // Cannot win while still holding a disc
    if (heldDisc.has_value()) {
        return false;
    }

    // Tower of Hanoi win condition: All discs must be on Tower_2 (index 1) OR Tower_3 (index 2)
    // Player wins by moving all discs from Tower_1 to either Tower_2 or Tower_3
    if ((pegStacks[1].size() == totalDiscs && totalDiscs > 0) || 
        (pegStacks[2].size() == totalDiscs && totalDiscs > 0)) {
        return true;
    }

    return false;
}

/**
 * Returns elapsed game time in seconds
 * @return Elapsed time in seconds, or 0.0 if timer hasn't started
 */
float Hanoi::getElapsedTime() const {
    if (!timerStarted) return 0.0f;
    
    auto currentTime = std::chrono::steady_clock::now();
    std::chrono::time_point<std::chrono::steady_clock> endTime;
    
    // If game is won, use saved end time, otherwise use current time
    if (isWon) {
        endTime = gameEndTime;
    } else {
        endTime = currentTime;
    }
    
    std::chrono::duration<float> elapsedTime = endTime - gameStartTime;
    return elapsedTime.count();
}

/**
 * Saves the current game state for undo/redo functionality
 * @return GameState snapshot of current game
 */
GameState Hanoi::saveState() const {
    GameState state;
    
    // Save peg stacks (only size ranks, not the full DiscState)
    for (int i = 0; i < 3; i++) {
        state.pegStacks[i].clear();
        for (const auto& disc : pegStacks[i]) {
            state.pegStacks[i].push_back(disc.sizeRank);
        }
    }
    
    // Save other state variables
    state.selectedPeg = selectedPeg;
    state.hasHeldDisc = heldDisc.has_value();
    state.heldDiscSizeRank = heldDisc.has_value() ? heldDisc->sizeRank : -1;
    state.heldDiscSourcePeg = heldDiscSourcePeg;
    state.moveCount = moveCount;
    state.isWon = isWon;
    
    return state;
}

/**
 * Restores game state from a saved snapshot
 * @param state GameState to restore
 */
void Hanoi::restoreState(const GameState& state) {
    // First, save all current discs before clearing (to preserve DiscState objects)
    std::vector<DiscState> allDiscs;
    for (int i = 0; i < 3; i++) {
        for (const auto& disc : pegStacks[i]) {
            allDiscs.push_back(disc);
        }
    }
    if (heldDisc.has_value()) {
        allDiscs.push_back(*heldDisc);
    }
    
    // Create map from sizeRank to DiscState
    std::map<int, DiscState> discBySize;
    for (const auto& disc : allDiscs) {
        discBySize[disc.sizeRank] = disc;
    }
    
    // Now clear current state
    for (int i = 0; i < 3; i++) {
        pegStacks[i].clear();
    }
    heldDisc.reset();
    
    // Restore peg stacks
    for (int i = 0; i < 3; i++) {
        pegStacks[i].clear();
        for (int sizeRank : state.pegStacks[i]) {
            if (discBySize.find(sizeRank) != discBySize.end()) {
                DiscState disc = discBySize[sizeRank];
                pegStacks[i].push_back(disc);
            } else {
                std::cout << "[UNDO/REDO] WARNING: Disc with sizeRank " << sizeRank 
                          << " not found in disc map!" << std::endl;
            }
        }
        // Update visual positions for all discs on this peg
        for (size_t stackIndex = 0; stackIndex < pegStacks[i].size(); stackIndex++) {
            placeDisc(pegStacks[i][stackIndex], i, static_cast<int>(stackIndex));
        }
    }
    
    // Restore other state first (needed for held disc positioning)
    selectedPeg = state.selectedPeg;
    moveCount = state.moveCount;
    isWon = state.isWon;
    
    // Restore held disc
    if (state.hasHeldDisc && state.heldDiscSizeRank > 0) {
        if (discBySize.find(state.heldDiscSizeRank) != discBySize.end()) {
            heldDisc = discBySize[state.heldDiscSizeRank];
            heldDiscSourcePeg = state.heldDiscSourcePeg;
            // Update visual position of held disc immediately
            if (heldDisc.has_value()) {
                auto& stack = pegStacks[selectedPeg];
                int topIndex = static_cast<int>(stack.size());
                glm::vec3 position = pegPositions[selectedPeg];
                float lift = 5.0f;
                position.y = baseLevelY + stackStepY * static_cast<float>(topIndex) + lift;
                glm::mat4 transform = heldDisc->baseTransform;
                transform[3] = glm::vec4(position, 1.0f);
                heldDisc->node->setLocalMatrix(transform);
                std::cout << "[UNDO/REDO] Restored held disc (sizeRank=" << state.heldDiscSizeRank 
                          << ") on peg " << selectedPeg << std::endl;
            }
        } else {
            std::cout << "[UNDO/REDO] WARNING: Held disc with sizeRank " << state.heldDiscSizeRank 
                      << " not found in disc map!" << std::endl;
        }
    } else {
        heldDisc.reset();
        heldDiscSourcePeg = -1;
    }
    
    // Update selection visuals
    updateSelectionVisuals();
    
    std::cout << "[UNDO/REDO] State restored. Move count: " << moveCount 
              << ", Held disc: " << (heldDisc.has_value() ? "yes" : "no")
              << ", Peg stacks sizes: [" << pegStacks[0].size() << ", " 
              << pegStacks[1].size() << ", " << pegStacks[2].size() << "]" << std::endl;
}

/**
 * Undoes the last move
 */
void Hanoi::undo() {
    if (undoStack.empty()) {
        std::cout << "[UNDO] No moves to undo" << std::endl;
        return;
    }
    
    // Save current state to redo stack
    redoStack.push_back(saveState());
    
    // Restore previous state
    GameState previousState = undoStack.back();
    undoStack.pop_back();
    restoreState(previousState);
    
    std::cout << "[UNDO] Move undone. Can undo " << undoStack.size() << " more times" << std::endl;
}

/**
 * Redoes the last undone move
 */
void Hanoi::redo() {
    if (redoStack.empty()) {
        std::cout << "[REDO] No moves to redo" << std::endl;
        return;
    }
    
    // Save current state to undo stack
    undoStack.push_back(saveState());
    
    // Restore next state
    GameState nextState = redoStack.back();
    redoStack.pop_back();
    restoreState(nextState);
    
    std::cout << "[REDO] Move redone. Can redo " << redoStack.size() << " more times" << std::endl;
}
