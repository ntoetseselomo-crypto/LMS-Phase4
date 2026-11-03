
#include <catch2/catch_test_macros.hpp>
#include "Repositories.h"
#include <sqlite3.h>
#include <stdexcept>
#include <string>

class LMSFixture {
public:
    Database db{":memory:"};
    LMSRepository repo{db};

    int lecturerId;
    int studentId;
    int hodId;
    int courseId;
    int assignmentId;
    int submissionId;

    LMSFixture() {
        repo.initialiseSchema();

        db.execute(
            "INSERT INTO Department (name) "
            "VALUES ('Computer Science');"
        );

        int departmentId = queryInt(
            "SELECT department_id FROM Department LIMIT 1;"
        );

        lecturerId = insertUser(
            "lecturer", "hash", "Lecturer", departmentId
        );

        studentId = insertUser(
            "student", "hash", "Student", departmentId
        );

        hodId = insertUser(
            "hod", "hash", "HoD", departmentId
        );

        db.execute(
            "INSERT INTO Course "
            "(course_name, department_id, lecturer_id) "
            "VALUES ('Computer Science', " +
            std::to_string(departmentId) + ", " +
            std::to_string(lecturerId) + ");"
        );

        courseId = queryInt(
            "SELECT course_id FROM Course LIMIT 1;"
        );

        assignmentId = repo.createAssignment(
            courseId,
            "Test Assignment",
            "Integration test",
            "2026-11-01 23:59:00",
            100.0,
            lecturerId
        );

        repo.submit(
            assignmentId,
            studentId,
            "assignment.pdf",
            "2026-10-30 12:00:00",
            "Submitted"
        );

        submissionId = queryInt(
            "SELECT submission_id FROM Submission LIMIT 1;"
        );
    }

    int insertUser(
        const std::string& username,
        const std::string& passwordHash,
        const std::string& role,
        int departmentId
    ) {
        sqlite3_stmt* stmt = nullptr;

        const char* sql =
            "INSERT INTO Users "
            "(username, password_hash, role, department_id) "
            "VALUES (?, ?, ?, ?);";

        if (sqlite3_prepare_v2(
            db.handle(), sql, -1, &stmt, nullptr
        ) != SQLITE_OK) {
            throw std::runtime_error(sqlite3_errmsg(db.handle()));
        }

        sqlite3_bind_text(
            stmt, 1, username.c_str(), -1, SQLITE_TRANSIENT
        );
        sqlite3_bind_text(
            stmt, 2, passwordHash.c_str(), -1, SQLITE_TRANSIENT
        );
        sqlite3_bind_text(
            stmt, 3, role.c_str(), -1, SQLITE_TRANSIENT
        );
        sqlite3_bind_int(stmt, 4, departmentId);

        if (sqlite3_step(stmt) != SQLITE_DONE) {
            std::string error = sqlite3_errmsg(db.handle());
            sqlite3_finalize(stmt);
            throw std::runtime_error(error);
        }

        sqlite3_finalize(stmt);

        return static_cast<int>(
            sqlite3_last_insert_rowid(db.handle())
        );
    }

    int queryInt(const std::string& sql) {
        sqlite3_stmt* stmt = nullptr;

        if (sqlite3_prepare_v2(
            db.handle(), sql.c_str(), -1, &stmt, nullptr
        ) != SQLITE_OK) {
            throw std::runtime_error(sqlite3_errmsg(db.handle()));
        }

        int result = -1;

        if (sqlite3_step(stmt) == SQLITE_ROW) {
            result = sqlite3_column_int(stmt, 0);
        }

        sqlite3_finalize(stmt);
        return result;
    }

    double queryDouble(const std::string& sql) {
        sqlite3_stmt* stmt = nullptr;

        if (sqlite3_prepare_v2(
            db.handle(), sql.c_str(), -1, &stmt, nullptr
        ) != SQLITE_OK) {
            throw std::runtime_error(sqlite3_errmsg(db.handle()));
        }

        double result = -1.0;

        if (sqlite3_step(stmt) == SQLITE_ROW) {
            result = sqlite3_column_double(stmt, 0);
        }

        sqlite3_finalize(stmt);
        return result;
    }

    std::string queryText(const std::string& sql) {
        sqlite3_stmt* stmt = nullptr;

        if (sqlite3_prepare_v2(
            db.handle(), sql.c_str(), -1, &stmt, nullptr
        ) != SQLITE_OK) {
            throw std::runtime_error(sqlite3_errmsg(db.handle()));
        }

        std::string result;

        if (sqlite3_step(stmt) == SQLITE_ROW) {
            const unsigned char* value =
                sqlite3_column_text(stmt, 0);

            if (value != nullptr) {
                result = reinterpret_cast<const char*>(value);
            }
        }

        sqlite3_finalize(stmt);
        return result;
    }
};

TEST_CASE("LMS repository integration tests", "[integration]") {

    SECTION("Assignment can be created and retrieved") {
        LMSFixture test;

        auto assignments = test.repo.assignments();

        REQUIRE(assignments.size() == 1);
        REQUIRE(assignments[0].title == "Test Assignment");
        REQUIRE(assignments[0].maxMark == 100.0);
    }

    SECTION("Submission is saved") {
        LMSFixture test;

        auto submissions = test.repo.submissions();

        REQUIRE(submissions.size() == 1);
        REQUIRE(submissions[0].studentId == test.studentId);
        REQUIRE(submissions[0].filePath == "assignment.pdf");
    }

    SECTION("Resubmission creates another submission version") {
        LMSFixture test;

        test.repo.submit(
            test.assignmentId,
            test.studentId,
            "assignment_v2.pdf",
            "2026-10-31 12:00:00",
            "Submitted"
        );

        REQUIRE(test.queryInt(
            "SELECT COUNT(*) FROM Submission;"
        ) == 2);

        REQUIRE(test.queryInt(
            "SELECT MAX(version) FROM Submission;"
        ) == 2);
    }

    SECTION("Saving a mark creates an audit record") {
        LMSFixture test;

        test.repo.saveMark(
            test.submissionId,
            test.lecturerId,
            75.0,
            "Good work"
        );

        REQUIRE(test.queryInt(
            "SELECT COUNT(*) FROM Mark;"
        ) == 1);

        REQUIRE(test.queryDouble(
            "SELECT score FROM Mark;"
        ) == 75.0);

        REQUIRE(test.queryInt(
            "SELECT COUNT(*) FROM AuditLog;"
        ) == 1);
    }

    SECTION("Updating a mark records the old and new marks") {
        LMSFixture test;

        test.repo.saveMark(
            test.submissionId,
            test.lecturerId,
            70.0,
            "First mark"
        );

        test.repo.saveMark(
            test.submissionId,
            test.lecturerId,
            85.0,
            "Corrected mark"
        );

        REQUIRE(test.queryInt(
            "SELECT COUNT(*) FROM Mark;"
        ) == 1);

        REQUIRE(test.queryDouble(
            "SELECT score FROM Mark;"
        ) == 85.0);

        REQUIRE(test.queryInt(
            "SELECT COUNT(*) FROM AuditLog;"
        ) == 2);

        REQUIRE(test.queryDouble(
            "SELECT old_mark FROM AuditLog ORDER BY log_id DESC LIMIT 1;"
        ) == 70.0);

        REQUIRE(test.queryDouble(
            "SELECT new_mark FROM AuditLog ORDER BY log_id DESC LIMIT 1;"
        ) == 85.0);
    }

    SECTION("Moderation can approve a mark") {
        LMSFixture test;

        test.repo.saveMark(
            test.submissionId,
            test.lecturerId,
            80.0,
            "Good"
        );

        int markId = test.queryInt(
            "SELECT mark_id FROM Mark LIMIT 1;"
        );

        test.repo.moderate(
            markId, test.hodId, true, "Approved"
        );

        REQUIRE(test.queryText(
            "SELECT moderation_status FROM Mark;"
        ) == "Approved");

        REQUIRE(test.queryInt(
            "SELECT COUNT(*) FROM AuditLog;"
        ) == 2);
    }

    SECTION("Moderation can request changes") {
        LMSFixture test;

        test.repo.saveMark(
            test.submissionId,
            test.lecturerId,
            80.0,
            "Check this"
        );

        int markId = test.queryInt(
            "SELECT mark_id FROM Mark LIMIT 1;"
        );

        test.repo.moderate(
            markId, test.hodId, false, "Please review"
        );

        REQUIRE(test.queryText(
            "SELECT moderation_status FROM Mark;"
        ) == "ChangesRequested");
    }

    SECTION("Moderating a nonexistent mark throws an exception") {
        LMSFixture test;

        REQUIRE_THROWS(
            test.repo.moderate(
                999, test.hodId, true, "Test"
            )
        );
    }

    SECTION("Approved mark can be released to the student") {
        LMSFixture test;

        test.repo.saveMark(
            test.submissionId,
            test.lecturerId,
            90.0,
            "Excellent"
        );

        int markId = test.queryInt(
            "SELECT mark_id FROM Mark LIMIT 1;"
        );

        test.repo.moderate(
            markId, test.hodId, true, "Approved"
        );

        test.repo.release(
            markId, test.hodId, "Results released"
        );

        REQUIRE(test.queryInt(
            "SELECT released FROM Mark;"
        ) == 1);

        auto marks = test.repo.marksForStudent(test.studentId);

        REQUIRE(marks.size() == 1);
        REQUIRE(marks[0].score == 90.0);
        REQUIRE(marks[0].released == true);
    }

    SECTION("Unapproved mark cannot be released") {
        LMSFixture test;

        test.repo.saveMark(
            test.submissionId,
            test.lecturerId,
            60.0,
            "Pending"
        );

        int markId = test.queryInt(
            "SELECT mark_id FROM Mark LIMIT 1;"
        );

        REQUIRE_THROWS(
            test.repo.release(
                markId, test.hodId, "Attempt release"
            )
        );
    }

    SECTION("Student cannot retrieve unreleased marks") {
        LMSFixture test;

        test.repo.saveMark(
            test.submissionId,
            test.lecturerId,
            65.0,
            "Pending"
        );

        auto marks = test.repo.marksForStudent(test.studentId);

        REQUIRE(marks.empty());
    }

    SECTION("Granting an extension saves the new deadline") {
        LMSFixture test;

        test.repo.grantExtension(
            test.assignmentId,
            test.studentId,
            "2026-11-05 23:59:00",
            test.hodId
        );

        REQUIRE(test.queryInt(
            "SELECT COUNT(*) FROM Extension;"
        ) == 1);

        REQUIRE(test.queryText(
            "SELECT new_deadline FROM Extension;"
        ) == "2026-11-05 23:59:00");
    }

    SECTION("Granting another extension updates the existing extension") {
        LMSFixture test;

        test.repo.grantExtension(
            test.assignmentId,
            test.studentId,
            "2026-11-05 23:59:00",
            test.hodId
        );

        test.repo.grantExtension(
            test.assignmentId,
            test.studentId,
            "2026-11-07 23:59:00",
            test.lecturerId
        );

        REQUIRE(test.queryInt(
            "SELECT COUNT(*) FROM Extension;"
        ) == 1);

        REQUIRE(test.queryText(
            "SELECT new_deadline FROM Extension;"
        ) == "2026-11-07 23:59:00");
    }

    SECTION("Lecturer can grant a class-wide extension") {
        LMSFixture test;

        test.repo.grantClassExtension(
            test.assignmentId,
            "2026-11-05 23:59:00",
            test.lecturerId
        );

        REQUIRE(test.queryInt(
            "SELECT COUNT(*) FROM ClassExtension;"
        ) == 1);

        REQUIRE(test.queryText(
            "SELECT new_deadline FROM ClassExtension;"
        ) == "2026-11-05 23:59:00");
    }

    SECTION("A new class-wide extension updates the previous one") {
        LMSFixture test;

        test.repo.grantClassExtension(
            test.assignmentId,
            "2026-11-05 23:59:00",
            test.lecturerId
        );

        test.repo.grantClassExtension(
            test.assignmentId,
            "2026-11-07 23:59:00",
            test.lecturerId
        );

        REQUIRE(test.queryInt(
            "SELECT COUNT(*) FROM ClassExtension;"
        ) == 1);

        REQUIRE(test.queryText(
            "SELECT new_deadline FROM ClassExtension;"
        ) == "2026-11-07 23:59:00");
    }

    SECTION("Student assignment list displays the effective class deadline") {
        LMSFixture test;

        test.repo.grantClassExtension(
            test.assignmentId,
            "2026-11-05 23:59:00",
            test.lecturerId
        );

        auto assignments = test.repo.assignmentsForStudent(test.studentId);

        REQUIRE(assignments.size() == 1);
        REQUIRE(assignments[0].deadline == "2026-11-01 23:59:00");
        REQUIRE(assignments[0].effectiveDeadline == "2026-11-05 23:59:00");
    }

    SECTION("A lecturer cannot extend another lecturer's assignment") {
        LMSFixture test;

        REQUIRE_THROWS(
            test.repo.grantClassExtension(
                test.assignmentId,
                "2026-11-05 23:59:00",
                test.hodId
            )
        );

        REQUIRE(test.queryInt(
            "SELECT COUNT(*) FROM ClassExtension;"
        ) == 0);
    }

    SECTION("Users can be retrieved") {
        LMSFixture test;

        auto users = test.repo.users();

        REQUIRE(users.size() == 3);
        REQUIRE(users[0] == "hod (HoD)");
        REQUIRE(users[1] == "lecturer (Lecturer)");
        REQUIRE(users[2] == "student (Student)");
    }
}

TEST_CASE("User role conversion tests", "[users]") {

    SECTION("Student role converts to text") {
        REQUIRE(roleToString(Role::Student) == "Student");
        REQUIRE(roleFromString("Student") == Role::Student);
    }

    SECTION("Lecturer role converts to text") {
        REQUIRE(roleToString(Role::Lecturer) == "Lecturer");
        REQUIRE(roleFromString("Lecturer") == Role::Lecturer);
    }

    SECTION("HoD role converts to text") {
        REQUIRE(roleToString(Role::HoD) == "HoD");
        REQUIRE(roleFromString("HoD") == Role::HoD);
    }

    SECTION("Administrator role converts to text") {
        REQUIRE(roleToString(Role::Administrator) == "Administrator");
        REQUIRE(roleFromString("Administrator") == Role::Administrator);
    }

    SECTION("Invalid role text throws an exception") {
        REQUIRE_THROWS(roleFromString("InvalidRole"));
    }

    SECTION("Unknown role value throws an exception") {
        REQUIRE_THROWS(roleToString(static_cast<Role>(99)));
    }
}

TEST_CASE("User repository tests", "[users]") {

    SECTION("User can be created and retrieved") {
        LMSFixture test;
        UserRepository users(test.db);

        int id = users.create(
            "newstudent",
            "hashed_password",
            Role::Student
        );

        auto result = users.findByUsername("newstudent");

        REQUIRE(result.has_value());
        REQUIRE(result->id == id);
        REQUIRE(result->username == "newstudent");
        REQUIRE(result->passwordHash == "hashed_password");
        REQUIRE(result->role == Role::Student);
    }

    SECTION("Nonexistent username returns no result") {
        LMSFixture test;
        UserRepository users(test.db);

        auto result = users.findByUsername("unknownuser");

        REQUIRE_FALSE(result.has_value());
    }
}
