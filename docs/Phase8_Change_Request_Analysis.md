# Phase 8: Change Request Analysis

## 1. Change Request

**Change Request ID:** CR1  
**Description:** A lecturer should be able to extend the deadline for a whole class, not just an individual student. The extension must be visible on the student's assignment list.

## 2. Maintenance Classification

This change is classified as **Perfective Maintenance**.

It improves an existing LMS feature by extending the individual deadline-extension functionality to support class-wide extensions. It also improves the student experience by displaying the effective deadline on the assignment list.

## 3. Impact Analysis

| Affected area | Impact of CR1 |
|---|---|
| Requirements | Extend the deadline-management requirement to support class-wide extensions and student visibility. |
| Use cases | Update the extension-management use case to include lecturer class-wide extensions and HoD/Administrator individual extensions. |
| User interface | Update menu option 9 and the student assignment list. |
| Repository classes | Add `grantClassExtension()` and `assignmentsForStudent()` to `LMSRepository`. |
| Data model | Add the `ClassExtension` table. Keep the existing individual `Extension` table. |
| Assignment data | Add `effectiveDeadline` to represent the deadline applicable to a student. |
| Testing | Add tests for class-wide extension creation, updating, student deadline display, and authorization. |

## 4. Implementation Scope

The change includes:

- Allowing lecturers to grant an extension for an assignment to the whole class.
- Allowing HoD and Administrator users to continue granting individual extensions.
- Displaying the effective deadline to students when an extension applies.
- Preventing lecturers from extending assignments belonging to courses they do not teach.
- Preserving the original assignment deadline and existing individual-extension functionality.

## 5. Expected Outcome

A lecturer can extend an assignment deadline for the whole class. Students see the effective deadline in their assignment list. Existing individual extensions remain available to authorized HoD and Administrator users.

## 6. Maintenance Decision

CR1 is implemented as a perfective maintenance change and released as LMS version 1.1.
