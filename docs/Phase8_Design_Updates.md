# Phase 8: Design Updates

## 1. Overview

This document records the design changes made to implement Change Request CR1: class-wide assignment deadline extensions.

The existing LMS architecture is retained. The changes are limited to the repository, database, assignment display, and extension-management menu.

## 2. Database Design

A new `ClassExtension` table stores class-wide extensions.

| Field | Description |
|---|---|
| `class_extension_id` | Primary key |
| `assignment_id` | Assignment receiving the extension; unique and references Assignment |
| `new_deadline` | New class-wide deadline |
| `granted_by` | User ID of the lecturer granting the extension; references Users |

The existing `Extension` table remains responsible for individual student extensions.

## 3. Repository Design

The `LMSRepository` class was extended with two methods:

### `grantClassExtension(int assignmentId, const std::string& newDeadline, int grantedBy)`

- Checks that the requesting user is a lecturer assigned to the course containing the assignment.
- Saves the class-wide deadline in `ClassExtension`.
- Updates the existing class extension if one already exists for the assignment.
- Rejects unauthorized requests.

### `assignmentsForStudent(int studentId)`

- Retrieves assignments for a student.
- Calculates the effective deadline using the original deadline, class-wide extension, and individual extension.
- Preserves the original assignment deadline.

## 4. Assignment Data Structure

The `Assignment` structure now includes:

`effectiveDeadline`

The original `deadline` field remains unchanged. The effective deadline represents the latest applicable deadline for the student.

## 5. User Interface Changes

### Lecturer

Menu option 9 allows lecturers to grant a class-wide extension by entering an assignment ID and a new deadline.

### HoD and Administrator

Menu option 9 continues to support individual student extensions.

### Student

The assignment list displays the extended deadline when the effective deadline is later than the original deadline.

## 6. Authorization

Only a lecturer assigned to the relevant course can grant a class-wide extension.

HoD and Administrator users retain the existing individual-extension functionality. Other roles cannot manage extensions through menu option 9.

## 7. Design Impact

The implementation adds class-wide extension functionality without replacing the existing individual-extension mechanism. The original assignment deadline remains available, and the effective deadline is calculated when retrieving a student's assignments.
