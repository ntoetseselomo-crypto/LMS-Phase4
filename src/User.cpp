#include "User.h"
#include <stdexcept>

std::string roleToString(Role role) {
    switch (role) {
    case Role::Student:
        return "Student";
    case Role::Lecturer:
        return "Lecturer";
    case Role::HoD:
        return "HoD";
    case Role::Administrator:
        return "Administrator";
    }

    throw std::runtime_error("Unknown role");
}

Role roleFromString(const std::string& value) {
    if (value == "Student") return Role::Student;
    if (value == "Lecturer") return Role::Lecturer;
    if (value == "HoD") return Role::HoD;
    if (value == "Administrator") return Role::Administrator;

    throw std::runtime_error("Invalid role stored in database");
}