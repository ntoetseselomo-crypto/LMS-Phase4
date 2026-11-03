# LMS Release Notes — Version 1.1

## 1. Release Information

**Product:** Learning Management System (LMS)  
**Version:** 1.1  
**Release type:** Maintenance release  
**Maintenance classification:** Perfective Maintenance

## 2. New Features

### Class-wide deadline extensions
- Lecturers can extend an assignment deadline for the whole class.
- Only the lecturer assigned to the relevant course can grant the extension.
- A new class-wide extension replaces the previous class-wide extension for that assignment.

### Student deadline visibility
- Students can view the effective assignment deadline in their assignment list.
- The original assignment deadline is preserved.
- Class-wide and individual extensions are considered when calculating the effective deadline.

## 3. Existing Functionality Retained

- HoD and Administrator users can still grant individual student extensions.
- Existing submission, authentication, marking, moderation, and release functionality remains available.
- The existing individual `Extension` table is retained.

## 4. Database Changes

A new `ClassExtension` table stores class-wide extension records, including the assignment, new deadline, and lecturer who granted the extension.

## 5. Verification

- C++ build: Passed.
- Regression testing: 6/6 CTest groups passed.
- Four new class-wide extension test sections passed within the repository integration tests.

## 6. Release Summary

Version 1.1 improves LMS deadline management by introducing class-wide extensions while preserving existing individual-extension functionality.
