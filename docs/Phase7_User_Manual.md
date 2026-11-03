# Phase 7: Learning Management System User Manual

## 1. Introduction

This manual explains how students, lecturers, Heads of Department (HoD) and administrators use the Learning Management System (LMS).

The LMS is a C++17 console application. Users interact with it through a text-based menu.

## 2. Starting the Application

1. Open the MSYS2 UCRT64 terminal.
2. Navigate to the LMS project directory.
3. Start the application:

   ```bash
   ./build/lms.exe
   ```

4. Log in using your authorised account.
5. The system displays the available menu options according to the user's role.

## 3. General Menu Options

| Menu option | Operation |
|---|---|
| 1 | List assignments |
| 2 | List submissions |
| 3 | List users (Administrator) |
| 4 | Submit work (Student) |
| 5 | Create assignment (Lecturer/Administrator) |
| 6 | Mark submission (Lecturer) |
| 7 | Moderate mark (HoD) |
| 8 | Release approved mark (HoD) |
| 9 | Grant extension (HoD/Administrator) |
| 10 | View my released marks (Student) |
| 0 | Logout |

## 4. Student User Manual

### 4.1 Viewing Assignments

1. Log in using a Student account.
2. Select option `1` to list assignments.
3. Review the available assignment information.

### 4.2 Submitting Work

1. Select option `4` (Submit work).
2. Enter the assignment ID.
3. Enter the submitted-file path.
4. Enter the submission timestamp in `YYYY-MM-DD HH:MM:SS` format.
5. Enter the effective deadline, including any extension granted.
6. Review the submission status displayed by the system.

### 4.3 Viewing Released Marks

1. Select option `10` (View my released marks).
2. Review marks released for your account.

Students cannot mark or moderate submissions.

## 5. Lecturer User Manual

### 5.1 Viewing Submissions

1. Log in using a Lecturer account.
2. Select option `2` to list submissions.

### 5.2 Creating an Assignment

1. Select option `5` (Create assignment).
2. Enter the course ID.
3. Enter the assignment title.
4. Enter the assignment description.
5. Enter the deadline.
6. Enter the maximum mark.
7. Submit the information.

The course ID must already exist in the system.

### 5.3 Marking a Submission

1. Select option `6` (Mark submission).
2. Enter the submission ID.
3. Enter the raw mark.
4. Enter feedback or a reason.
5. Submit the marking information.

The mark is subject to moderation and release.

## 6. Head of Department User Manual

### 6.1 Moderating a Mark

1. Log in using an HoD account.
2. Select option `7` (Moderate mark).
3. Enter the mark ID.
4. Choose whether to approve the mark (`y` or `n`).
5. Enter the moderation reason.
6. Submit the decision.

### 6.2 Releasing an Approved Mark

1. Select option `8` (Release approved mark).
2. Enter the mark ID.
3. Enter the release reason.
4. Submit the release action.

### 6.3 Granting an Extension

1. Select option `9` (Grant extension).
2. Enter the assignment ID.
3. Enter the student user ID.
4. Enter the new deadline.
5. Submit the extension information.

The current implementation supports individual student extensions.

## 7. Administrator User Manual

### 7.1 Viewing Users

1. Log in using an Administrator account.
2. Select option `3` (List users).
3. Review the displayed user information.

### 7.2 Creating an Assignment

1. Select option `5` (Create assignment).
2. Enter the course ID.
3. Enter the assignment title and description.
4. Enter the deadline.
5. Enter the maximum mark.
6. Submit the information.

### 7.3 Granting an Extension

1. Select option `9` (Grant extension).
2. Enter the assignment ID.
3. Enter the student user ID.
4. Enter the new deadline.
5. Submit the extension information.

## 8. Logging Out

1. Select option `0` (Logout).
2. The current user session ends.

## 9. User Responsibilities and Security

- Keep login credentials private.
- Use only functions permitted for your role.
- Enter accurate assignment and submission information.
- Protect submitted academic work and student records.
- Report unexpected application behaviour to the system administrator.

## 10. Important System Limitations

- The LMS uses a console-based interface.
- Students enter the submission timestamp and effective deadline during submission.
- The current extension function supports individual students, not an entire class.
- This manual describes the current implemented menu options.
