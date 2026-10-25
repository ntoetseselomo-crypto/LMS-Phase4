#include "User.h"
#include <stdexcept>
std::string roleToString(Role r) {
    switch (r) {
    case Role::Student: return "Student";
    case Role::Lecturer: return "Lecturer";
    case Role::HoD: return "HoD";
    case Role::Administrator: return "Administrator";
    }
    throw std::runtime_error("Unknown role");
}
Role roleFromString(const std::string& s) {
    if (s == "Student") return Role::Student;
    if (s == "Lecturer") return Role::Lecturer;
    if (s == "HoD") return Role::HoD;
    if (s == "Administrator") return Role::Administrator;
    throw std::runtime_error("Invalid role stored in database");
}
