#include <catch2/catch_test_macros.hpp>
#include "Services.h"
#include <stdexcept>

// Authentication tests
TEST_CASE("Authentication service tests", "[services][auth]") {

    SECTION("Empty username is rejected") {
        Database db(":memory:");
        db.execute(
            "CREATE TABLE Users ("
            "user_id INTEGER PRIMARY KEY AUTOINCREMENT,"
            "username TEXT UNIQUE NOT NULL,"
            "password_hash TEXT NOT NULL,"
            "role TEXT NOT NULL,"
            "department_id INTEGER);"
        );

        UserRepository users(db);
        AuthService auth(users);

        REQUIRE_THROWS_AS(
            auth.registerUser("", "password123", Role::Student),
            std::invalid_argument
        );
    }

    SECTION("Username longer than 80 characters is rejected") {
        Database db(":memory:");
        db.execute(
            "CREATE TABLE Users ("
            "user_id INTEGER PRIMARY KEY AUTOINCREMENT,"
            "username TEXT UNIQUE NOT NULL,"
            "password_hash TEXT NOT NULL,"
            "role TEXT NOT NULL,"
            "department_id INTEGER);"
        );

        UserRepository users(db);
        AuthService auth(users);

        REQUIRE_THROWS_AS(
            auth.registerUser(std::string(81, 'a'), "password123", Role::Student),
            std::invalid_argument
        );
    }

    SECTION("Password shorter than 10 characters is rejected") {
        Database db(":memory:");
        db.execute(
            "CREATE TABLE Users ("
            "user_id INTEGER PRIMARY KEY AUTOINCREMENT,"
            "username TEXT UNIQUE NOT NULL,"
            "password_hash TEXT NOT NULL,"
            "role TEXT NOT NULL,"
            "department_id INTEGER);"
        );

        UserRepository users(db);
        AuthService auth(users);

        REQUIRE_THROWS_AS(
            auth.registerUser("student1", "short", Role::Student),
            std::invalid_argument
        );
    }

    SECTION("Valid registration and login succeed") {
        Database db(":memory:");
        db.execute(
            "CREATE TABLE Users ("
            "user_id INTEGER PRIMARY KEY AUTOINCREMENT,"
            "username TEXT UNIQUE NOT NULL,"
            "password_hash TEXT NOT NULL,"
            "role TEXT NOT NULL,"
            "department_id INTEGER);"
        );

        UserRepository users(db);
        AuthService auth(users);

        int id = auth.registerUser(
            "student1", "securePassword123", Role::Student
        );

        REQUIRE(id > 0);

        auto loggedIn = auth.login("student1", "securePassword123");

        REQUIRE(loggedIn != nullptr);
        REQUIRE(loggedIn->username() == "student1");
        REQUIRE(loggedIn->role() == Role::Student);
    }

    SECTION("Incorrect password is rejected") {
        Database db(":memory:");
        db.execute(
            "CREATE TABLE Users ("
            "user_id INTEGER PRIMARY KEY AUTOINCREMENT,"
            "username TEXT UNIQUE NOT NULL,"
            "password_hash TEXT NOT NULL,"
            "role TEXT NOT NULL,"
            "department_id INTEGER);"
        );

        UserRepository users(db);
        AuthService auth(users);

        auth.registerUser(
            "student1", "securePassword123", Role::Student
        );

        auto loggedIn = auth.login("student1", "wrongPassword123");

        REQUIRE(loggedIn == nullptr);
    }

    SECTION("Unknown username is rejected") {
        Database db(":memory:");
        db.execute(
            "CREATE TABLE Users ("
            "user_id INTEGER PRIMARY KEY AUTOINCREMENT,"
            "username TEXT UNIQUE NOT NULL,"
            "password_hash TEXT NOT NULL,"
            "role TEXT NOT NULL,"
            "department_id INTEGER);"
        );

        UserRepository users(db);
        AuthService auth(users);

        auto loggedIn = auth.login("unknown", "password123");

        REQUIRE(loggedIn == nullptr);
    }
}

// Submission service tests
TEST_CASE("Submission service tests", "[services][submission]") {

    SECTION("Submission before deadline is classified as submitted") {
        Database db(":memory:");
        LMSRepository repo(db);
        LatePenaltyStrategy strategy;
        SubmissionService service(repo, strategy);

        REQUIRE(
            service.classify(
                "2026-10-30 12:00:00",
                "2026-10-31 23:59:00",
                ""
            ) == "Submitted"
        );
    }

    SECTION("Submission after deadline is classified as late") {
        Database db(":memory:");
        LMSRepository repo(db);
        LatePenaltyStrategy strategy;
        SubmissionService service(repo, strategy);

        REQUIRE(
            service.classify(
                "2026-11-02 12:00:00",
                "2026-10-31 23:59:00",
                ""
            ) == "Late"
        );
    }

    SECTION("Valid extension allows a later submission") {
        Database db(":memory:");
        LMSRepository repo(db);
        LatePenaltyStrategy strategy;
        SubmissionService service(repo, strategy);

        REQUIRE(
            service.classify(
                "2026-11-03 12:00:00",
                "2026-10-31 23:59:00",
                "2026-11-04 23:59:00"
            ) == "Submitted"
        );
    }

    SECTION("Submission after extended deadline is late") {
        Database db(":memory:");
        LMSRepository repo(db);
        LatePenaltyStrategy strategy;
        SubmissionService service(repo, strategy);

        REQUIRE(
            service.classify(
                "2026-11-05 12:00:00",
                "2026-10-31 23:59:00",
                "2026-11-04 23:59:00"
            ) == "Late"
        );
    }

    SECTION("Valid final mark is calculated") {
        Database db(":memory:");
        LMSRepository repo(db);
        LatePenaltyStrategy strategy;
        SubmissionService service(repo, strategy);

        REQUIRE(
            service.calculateFinalMark(80.0, 100.0, 2) == 64.0
        );
    }

    SECTION("Negative mark is rejected") {
        Database db(":memory:");
        LMSRepository repo(db);
        LatePenaltyStrategy strategy;
        SubmissionService service(repo, strategy);

        REQUIRE_THROWS_AS(
            service.calculateFinalMark(-5.0, 100.0, 0),
            std::invalid_argument
        );
    }

    SECTION("Mark above maximum is rejected") {
        Database db(":memory:");
        LMSRepository repo(db);
        LatePenaltyStrategy strategy;
        SubmissionService service(repo, strategy);

        REQUIRE_THROWS_AS(
            service.calculateFinalMark(110.0, 100.0, 0),
            std::invalid_argument
        );
    }

    SECTION("Zero maximum mark is rejected") {
        Database db(":memory:");
        LMSRepository repo(db);
        LatePenaltyStrategy strategy;
        SubmissionService service(repo, strategy);

        REQUIRE_THROWS_AS(
            service.calculateFinalMark(0.0, 0.0, 0),
            std::invalid_argument
        );
    }
}