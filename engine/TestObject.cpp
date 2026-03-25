#include "TestObject.h"
#include <iostream>
#include <cassert>
#include <string>

// ===================================================================
// HELPER CLASS FOR TESTING (REQUIRED)
// ===================================================================

/**
 * @brief Concrete dummy class derived from Object.
 *
 * This class is required in order to instantiate Object, which is
 * abstract due to the pure virtual render() method.
 * The render() implementation is intentionally empty and used
 * only to satisfy the compiler during testing.
 */
class TestableObject : public Object {
public:
    /**
     * @brief Constructs a TestableObject with the given name.
     *
     * @param name Name assigned to the object.
     */
    explicit TestableObject(const std::string& name) : Object(name) {}

    /**
     * @brief Dummy implementation of the pure virtual render() method.
     *
     * This method performs no operation and exists only to allow
     * instantiation of the abstract base class Object.
     */
    void render() override {
        // Intentionally left empty
    }
};

// ===================================================================
// TEST FUNCTION IMPLEMENTATION
// ===================================================================

namespace ObjectTest {

    /**
     * @brief Executes unit tests for the Object class.
     *
     * This function verifies:
     * - Correct assignment of the object name.
     * - Unique and sequential generation of object IDs.
     *
     * Assertions are used to ensure correctness.
     */
    void test() {
        std::cout << "Running Object tests..." << std::endl;

        // ------------------------------------
        // TEST 1: Name Assignment
        // ------------------------------------
        std::cout << "\tTesting name assignment..." << std::endl;
        const std::string expectedName = "Test_ID_Check_Object";
        TestableObject obj(expectedName);
        assert(obj.getName() == expectedName);
        std::cout << "\tName assignment test OK." << std::endl;

        // ------------------------------------
        // TEST 2: Unique and Sequential ID Generation
        // ------------------------------------
        std::cout << "\tTesting unique and sequential ID generation..." << std::endl;

        TestableObject obj1("ID_Check_1");
        TestableObject obj2("ID_Check_2");
        TestableObject obj3("ID_Check_3");

        const unsigned int id1 = obj1.getId();
        const unsigned int id2 = obj2.getId();
        const unsigned int id3 = obj3.getId();

        // Verify uniqueness
        assert(id1 != id2 && id2 != id3 && id1 != id3);

        // Verify sequential order
        assert(id2 == id1 + 1 && id3 == id2 + 1);
        std::cout << "\tID generation test OK." << std::endl;

        std::cout << "Object tests passed!" << std::endl;
    }

} // namespace ObjectTest