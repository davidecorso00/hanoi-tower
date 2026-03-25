#include "TestHanoi.h"

#include "Node.h"
#include "PerspectiveCamera.h"

#include <cassert>
#include <iostream>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <cmath>

namespace HanoiTest {

void test() {
    std::cout << "Running Hanoi tests..." << std::endl;

    // --------------------------------------------------
    // Setup minimal scene graph: 3 towers + 2 discs
    // --------------------------------------------------
    PerspectiveCamera camera("TestCamera", 45.0f, 4.0f / 3.0f, 1.0f, 1000.0f);
    Hanoi game(&camera, nullptr); // Engine is not required for these tests

    Node root("Root");
    Node tower1("Tower_1");
    Node tower2("Tower_2");
    Node tower3("Tower_3");

    root.addChildNode(&tower1);
    root.addChildNode(&tower2);
    root.addChildNode(&tower3);

    // Position the three towers along the X axis
    tower1.translate(glm::vec3(0.0f, 0.0f, 0.0f));
    tower2.translate(glm::vec3(10.0f, 0.0f, 0.0f));
    tower3.translate(glm::vec3(20.0f, 0.0f, 0.0f));

    // Create two discs stacked on Tower_1
    Node disk1("Disk_1"); // smaller
    Node disk2("Disk_2"); // larger
    tower1.addChildNode(&disk1);
    tower1.addChildNode(&disk2);

    disk1.translate(glm::vec3(0.0f, 0.0f, 0.0f));
    disk2.translate(glm::vec3(0.0f, 1.0f, 0.0f));

    // Initialize game state from the synthetic scene
    game.initHanoiState(&root);

    // --------------------------------------------------
    // Basic state checks after initialization
    // --------------------------------------------------
    assert(!game.isGameWon());
    assert(game.getMoveCount() == 0);
    assert(!game.hasHeldDisc());
    // Initial selected peg should be the one with most discs (Tower_1 -> index 0)
    assert(game.getSelectedPeg() == 0);

    // Elapsed time should be non-negative once the timer has started
    float elapsed = game.getElapsedTime();
    assert(elapsed >= 0.0f);

    // --------------------------------------------------
    // Camera helpers: presets and boundary clamping
    // --------------------------------------------------
    game.applyCameraPreset(0);
    {
        glm::mat4 worldMatrix = camera.getWorldMatrix();
        glm::vec3 camPos(worldMatrix[3]);

        // Expected preset 0 position as defined in Hanoi::buildCameraPresets
        glm::vec3 expected(5.0f, 69.0f, 80.0f);
        const float epsilon = 0.01f;
        assert(std::fabs(camPos.x - expected.x) <= epsilon);
        assert(std::fabs(camPos.y - expected.y) <= epsilon);
        assert(std::fabs(camPos.z - expected.z) <= epsilon);
    }

    // Move camera far outside allowed bounds and ensure it gets clamped
    {
        glm::mat4 m(1.0f);
        m = glm::translate(m, glm::vec3(1000.0f, 1000.0f, 1000.0f));
        camera.setLocalMatrix(m);

        game.checkBoundaries();

        glm::vec3 clampedPos = glm::vec3(camera.getLocalMatrix()[3]);
        assert(clampedPos.x <= MAP_LIMIT_X_R && clampedPos.x >= MAP_LIMIT_X_L);
        assert(clampedPos.y <= MAP_LIMIT_Y_R && clampedPos.y >= MAP_LIMIT_Y_L);
        assert(clampedPos.z <= MAP_LIMIT_Z_D && clampedPos.z >= MAP_LIMIT_Z_U);
    }

    // --------------------------------------------------
    // Interaction + undo/redo
    // --------------------------------------------------
    // Pick up a disc from the selected peg
    game.specialCallback(KEY_UP, 0, 0);
    assert(game.hasHeldDisc());
    assert(game.canUndo());

    // Undo should restore the previous state (no disc held)
    game.undo();
    assert(!game.hasHeldDisc());
    assert(game.canRedo());

    // Redo should re-apply the last undone state without crashing
    game.redo();

    std::cout << "Hanoi tests passed!" << std::endl;
}

} // namespace HanoiTest


