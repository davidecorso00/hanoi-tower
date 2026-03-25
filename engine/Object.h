#pragma once

#include "Lib.h"

#include <string>
#include <memory>
#include <atomic>

/**
 * @brief Base abstract class for all scene objects.
 *
 * Provides a unique ID, a name, and a pure virtual render method
 * that must be implemented by derived classes.
 */
class ENG_API Object {
public:
    /**
     * @brief Constructor that assigns a name to the object.
     * @param name Name of the object.
     */
    Object(const std::string& name);

    /**
     * @brief Virtual destructor.
     */
    virtual ~Object() = default;

    /**
     * @brief Renders the object.
     *        Must be overridden by derived classes.
     */
    virtual void render() = 0;

    /**
     * @brief Returns the object's name.
     * @return Object name as std::string.
     */
    virtual std::string getName() const;

    /**
     * @brief Returns the object's unique ID.
     * @return ID as unsigned int.
     */
    virtual unsigned int getId() const;

protected:
    unsigned int id;       ///< Unique ID of the object
    std::string name;      ///< Name of the object

private:
    static std::atomic<unsigned int> nextId; ///< Global counter for generating IDs
};