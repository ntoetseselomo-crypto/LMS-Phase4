#pragma once
#include <string>
#include <utility>

enum class Role { Student, Lecturer, HoD, Administrator };
std::string roleToString(Role role);
Role roleFromString(const std::string& value);

class User {
public:
    User(int id, std::string username, Role role)
        : id_(id), username_(std::move(username)), role_(role) {}
    virtual ~User() = default;
    int id() const noexcept { return id_; }
    const std::string& username() const noexcept { return username_; }
    Role role() const noexcept { return role_; }
    virtual std::string dashboard() const = 0;
private:
    int id_;
    std::string username_;
    Role role_;
};
class Student final : public User {
public: using User::User;
    std::string dashboard() const override { return "Student: view assignments and submit work."; }
};
class Lecturer final : public User {
public: using User::User;
    std::string dashboard() const override { return "Lecturer: create assignments and mark submissions."; }
};
class HeadOfDepartment final : public User {
public: using User::User;
    std::string dashboard() const override { return "HoD: moderate and release marks."; }
};
class Administrator final : public User {
public: using User::User;
    std::string dashboard() const override { return "Administrator: manage system users and courses."; }
};
