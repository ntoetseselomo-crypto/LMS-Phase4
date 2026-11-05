# Phase 6: Quality Assurance Report

## 1. Introduction

This report records the quality assurance activities performed on the
Learning Management System (LMS) developed using C++17, SQLite and libsodium.

The activities include automated testing, static analysis, code metrics
and a self-code review.

## 2. Automated Testing

The project was built using CMake and Ninja.

Build command:

    cmake --build build

Test command:

    ctest --test-dir build --output-on-failure

### Test Results

| Test | Result |
|---|---|
| Authentication service tests | Passed |
| Submission service tests | Passed |
| LMS repository integration tests | Passed |
| User role conversion tests | Passed |
| User repository tests | Passed |
| Late penalty rules | Passed |

Total tests: 6

Passed: 6

Failed: 0

Success rate: 100%

Total execution time: 0.69 seconds.

## 3. Static Analysis

Cppcheck version 2.21.0 was used to inspect the C++ source files.

Command:

    cppcheck --enable=all --inconclusive --check-level=exhaustive --std=c++17 --language=c++ --suppress=missingIncludeSystem -I include src 2> cppcheck-report.txt

Five source files were analysed.

### Findings

Cppcheck reported two style warnings:

- SubmissionService::classify() was reported as unused.
- SubmissionService::calculateFinalMark() was reported as unused.

These functions are declared and defined in the project and are called
by tests in tests/test_services.cpp. The warnings occur because the
static-analysis command scans src but does not include tests.

The functions were therefore retained.

No critical errors were reported in the final analysis.

The report is saved as cppcheck-report.txt.

## 4. Code Metrics

The project was measured using scc version 4.1.0.

Command:

    scc src include tests

| Metric | Result |
|---|---:|
| Files analysed | 12 |
| Total lines | 1,769 |
| Code lines | 1,451 |
| Blank lines | 312 |
| Comment lines | 6 |
| Aggregate complexity | 249 |

### Cyclomatic Complexity

Lizard version 1.25.0 was used to calculate function-level cyclomatic
complexity.

Command:

    lizard src include tests

| Function | File | CCN |
|---|---|---:|
| menu() | src/main.cpp | 36 |
| validDateTime() | src/main.cpp | 12 |
| main() | src/main.cpp | 8 |
| AuthService::login() | src/Services.cpp | 8 |
| AuthService::registerUser() | src/Services.cpp | 6 |
| LMSRepository::saveMark() | src/Repositories.cpp | 5 |
| LMSRepository::moderate() | src/Repositories.cpp | 5 |
| SubmissionService::calculateFinalMark() | src/Services.cpp | 4 |
| SubmissionService::classify() | src/Services.cpp | 4 |
| LatePenaltyStrategy::apply() | src/Services.cpp | 3 |

Lizard analysed 61 functions and reported an average CCN of 3.2.

### Complexity Observation

The menu() function has a cyclomatic complexity of 36, exceeding
Lizard's warning threshold of 15.

This indicates that the function contains many decision paths.
It is a candidate for future refactoring into smaller functions
to improve readability, maintainability and testability.

The complexity report is saved as lizard-report.txt.

## 5. Self-Code Review

The following checklist was used during the review:

| Review item | Result |
|---|---|
| Project builds successfully | Passed |
| Automated tests pass | Passed |
| Struct fields have safe default initialisation | Reviewed and improved |
| Function parameter names are consistent | Reviewed and improved |
| Const-correctness is applied where appropriate | Reviewed and improved |
| Input validation is present in authentication and marking logic | Reviewed |
| Passwords are hashed using libsodium | Reviewed |
| Database operations use the repository/data-access layer | Reviewed |
| High-complexity functions identified | Passed |
| Static-analysis findings documented | Passed |

### Changes Made

The quality review resulted in the following improvements:

1. Added default member initialisation to repository data structures.
2. Aligned parameter names between repository declarations and definitions.
3. Improved const-correctness in the main menu and collection loops.
4. Made SubmissionService::classify() static because it does not require
   object state.
5. Rebuilt the application and reran the automated test suite.

These changes were followed by a successful build and six passing tests.

## 6. Continuous Integration

The repository contains a GitHub Actions workflow that:

1. Checks out the repository.
2. Installs the required C++ build and database dependencies.
3. Configures the project using CMake and Ninja.
4. Builds the LMS application and tests.
5. Executes the automated test suite using CTest.

The workflow is configured to run on pushes and pull requests
targeting the main and master branches.

## 7. Remaining Improvement Opportunities

The following improvements are recommended for future maintenance:

- Refactor the high-complexity menu() function.
- Increase automated test coverage where necessary.
- Review and reduce duplicated logic where appropriate.
- Expand code documentation where it would improve maintainability.

## 8. Conclusion

The LMS project successfully builds and passes all six automated tests.

Static analysis was performed using Cppcheck, while code size and
complexity were measured using scc and Lizard.

The review identified one high-complexity function for future improvement.
The findings and generated analysis reports are retained in the repository.

The project has therefore undergone documented quality assurance
activities as part of Phase 6.
