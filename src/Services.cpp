
#include "Services.h"
#include <sodium.h>
#include <stdexcept>
#include <algorithm>
#include <cctype>

int AuthService::registerUser(const std::string& username,
                              const std::string& password,
                              Role role) {
    if (username.empty() || username.size() > 80) {
        throw std::invalid_argument("Username must be 1-80 characters");
    }

    if (password.size() < 10) {
        throw std::invalid_argument("Password must contain at least 10 characters");
    }

    if (sodium_init() < 0) {
        throw std::runtime_error("Could not initialise libsodium");
    }

    char hash[crypto_pwhash_STRBYTES];

    if (crypto_pwhash_str(
            hash,
            password.c_str(),
            password.size(),
            crypto_pwhash_OPSLIMIT_INTERACTIVE,
            crypto_pwhash_MEMLIMIT_INTERACTIVE) != 0) {
        throw std::runtime_error("Password hashing failed (insufficient memory)");
    }

    return users_.create(username, hash, role);
}

std::unique_ptr<User> AuthService::login(
    const std::string& username,
    const std::string& password) {

    if (sodium_init() < 0) {
        throw std::runtime_error("Could not initialise libsodium");
    }

    auto row = users_.findByUsername(username);

    if (!row || crypto_pwhash_str_verify(
                    row->passwordHash.c_str(),
                    password.c_str(),
                    password.size()) != 0) {
        return nullptr;
    }

    switch (row->role) {
    case Role::Student:
        return std::make_unique<Student>(row->id, row->username, row->role);

    case Role::Lecturer:
        return std::make_unique<Lecturer>(row->id, row->username, row->role);

    case Role::HoD:
        return std::make_unique<HeadOfDepartment>(
            row->id, row->username, row->role);

    case Role::Administrator:
        return std::make_unique<Administrator>(
            row->id, row->username, row->role);
    }

    return nullptr;
}

double LatePenaltyStrategy::apply(double rawMark, int daysLate) const {
    if (daysLate <= 0) {
        return rawMark;
    }

    if (daysLate > 3) {
        return 0.0;
    }

    return std::max(
        0.0,
        rawMark * (1.0 - percentPerDay_ * daysLate));
}

std::string SubmissionService::classify(
    const std::string& submittedAt,
    const std::string& deadline,
    const std::string& effectiveDeadline) {

    const std::string& limit =
        effectiveDeadline.empty() || effectiveDeadline < deadline
            ? deadline
            : effectiveDeadline;

    if (submittedAt <= limit) {
        return "Submitted";
    }

    return "Late";
}

double SubmissionService::calculateFinalMark(
    double rawMark,
    double maxMark,
    int daysLate) const {

    if (rawMark < 0 || maxMark <= 0 || rawMark > maxMark) {
        throw std::invalid_argument(
            "Mark must be between zero and maximum mark");
    }

    return strategy_.apply(rawMark, daysLate);
}
