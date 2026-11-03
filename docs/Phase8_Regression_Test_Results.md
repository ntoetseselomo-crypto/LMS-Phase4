# Phase 8: Regression Test Results

## 1. Purpose

Regression testing was performed to verify that the class-wide deadline extension change works correctly and that existing LMS functionality remains operational.

## 2. New Test Cases

| Test case | Expected result | Result |
|---|---|---|
| Lecturer grants a class-wide extension | Extension is saved in `ClassExtension`. | Passed |
| New class-wide extension updates the previous one | Existing extension is updated; no duplicate is created. | Passed |
| Student assignment list displays the effective class deadline | Original deadline remains unchanged and effective deadline is displayed. | Passed |
| Unauthorized role attempts a class-wide extension | Request is rejected and no extension is saved. | Passed |

## 3. Build Verification

Command:

`cmake --build build`

**Result:** Passed. The LMS executable was successfully linked.

## 4. Regression Test Execution

Command:

`ctest --test-dir build --output-on-failure`

**Result:** All 6 CTest test groups passed.

| Test group | Result |
|---|---|
| Authentication service tests | Passed |
| Submission service tests | Passed |
| LMS repository integration tests | Passed |
| User role conversion tests | Passed |
| User repository tests | Passed |
| Late penalty rules | Passed |

**Overall result:** 6/6 test groups passed (100%).

## 5. Conclusion

The build completed successfully and all existing CTest groups passed after the Phase 8 implementation. The four new class-wide extension test sections also passed as part of the LMS repository integration tests.

The regression results indicate that the implemented change did not break the tested existing functionality.
