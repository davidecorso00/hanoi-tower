#include "Object.h"

// Global counter initialization
std::atomic<unsigned int> Object::nextId{ 0 };

// ------------------------------------------------------------------
// Constructor
// ------------------------------------------------------------------
ENG_API Object::Object(const std::string& name) : name(name) { this->id = nextId++; }


// ------------------------------------------------------------------
// Getters
// ------------------------------------------------------------------
unsigned int Object::getId() const {
    return this->id;
}

std::string Object::getName() const {
    return this->name;
}