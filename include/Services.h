#pragma once
#include "Repositories.h"
#include <memory>
#include <string>

class AuthService {
public:
    explicit AuthService(UserRepository& users) : users_(users) {}
    int registerUser(const std::string& username, const std::string& password, Role role);
    std::unique_ptr<User> login(const std::string& username, const std::string& password);
private: UserRepository& users_;
};
class IPenaltyStrategy {
public: virtual ~IPenaltyStrategy() = default;
    virtual double apply(double rawMark, int daysLate) const = 0;
};
class LatePenaltyStrategy final : public IPenaltyStrategy {
public:
    // ASSUMPTION: 10% of the available mark per late day, up to 3 days.
    // Replace this rate with the exact rate specified in the Phase 2 SRS.
    explicit LatePenaltyStrategy(double percentPerDay = 0.10) : percentPerDay_(percentPerDay) {}
    double apply(double rawMark, int daysLate) const override;
private: double percentPerDay_;
};
class SubmissionService {
public:
    SubmissionService(LMSRepository& repo, const IPenaltyStrategy& strategy)
      : repo_(repo), strategy_(strategy) {}
   static std::string classify(const std::string& submittedAt,
                            const std::string& deadline,
                            const std::string& effectiveDeadline);
    double calculateFinalMark(double rawMark, double maxMark, int daysLate) const;
private: LMSRepository& repo_; const IPenaltyStrategy& strategy_;
};
