#pragma once
#include "Database.h"
#include "User.h"
#include <memory>
#include <optional>
#include <string>
#include <vector>

struct Assignment {
    int id{};
    std::string title;
    std::string deadline; // ISO 8601: YYYY-MM-DD HH:MM:SS
    double maxMark{};
};

struct Submission {
    int id{};
    int assignmentId{};
    int studentId{};
    std::string submittedAt;
    std::string filePath;
    std::string status;
};

struct MarkRecord {
    int id{};
    int submissionId{};
    double score{};
    std::string feedback;
    std::string moderationStatus;
    bool released{};
};

class UserRepository {
public:
    explicit UserRepository(Database& db) : db_(db) {}

    int create(const std::string& username, const std::string& passwordHash, Role role);

    struct LoginRow {
        int id{};
        std::string username;
        std::string passwordHash;
        Role role{Role::Student};
    };

    std::optional<LoginRow> findByUsername(const std::string& username);

private:
    Database& db_;
};

class LMSRepository {
public:
    explicit LMSRepository(Database& db) : db_(db) {}

    void initialiseSchema();

    int createAssignment(int courseId, const std::string& title, const std::string& description,
                         const std::string& deadline, double maxMark, int creatorId);

    std::vector<Assignment> assignments();

    void submit(int assignmentId, int studentId, const std::string& filePath,
                const std::string& submittedAt, const std::string& status);

    std::vector<Submission> submissions();

    void saveMark(int submissionId, int lecturerId, double score, const std::string& feedback);

    void moderate(int markId, int hodId, bool approve, const std::string& reason);

    void release(int markId, int hodId, const std::string& reason);

    void grantExtension(int assignmentId, int studentId, const std::string& newDeadline, int grantedBy);

    std::vector<MarkRecord> marksForStudent(int studentId);

    std::vector<std::string> users();

private:
    Database& db_;
};