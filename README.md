# LMS Phase 4 — C++17 implementation starter

This repository is a working-structure implementation based on the Phase 3 design: layered architecture, abstract `User` with four derived roles, Repository pattern, Strategy pattern for late penalties, SQLite persistence, parameterised SQL, and mark audit entries.

## Dependencies
- CMake 3.16+
- C++17 compiler
- SQLite development library
- libsodium development library (password hashes use libsodium's `crypto_pwhash_str`, which uses Argon2id in current libsodium)

### Windows
For a reliable CMake build, use MSYS2 or another toolchain that provides CMake, GCC/Clang, SQLite and libsodium. In MSYS2 UCRT64, install the matching packages (package names can vary by MSYS2 environment):
`mingw-w64-ucrt-x86_64-gcc`, `mingw-w64-ucrt-x86_64-cmake`, `mingw-w64-ucrt-x86_64-sqlite3`, `mingw-w64-ucrt-x86_64-libsodium`, `mingw-w64-ucrt-x86_64-pkgconf`.

Open the matching MSYS2 UCRT64 terminal in this directory:
```sh
cmake -S . -B build
cmake --build build
./build/lms.exe
```
If `pkg-config` cannot find libsodium, set `PKG_CONFIG_PATH` or adjust `CMakeLists.txt` to the installed library paths. Do not mix 32-bit and 64-bit libraries.

### Dev-C++ note
Dev-C++ 6.3 can compile C++ files, but this project uses CMake and external SQLite/libsodium development libraries. Configure the matching compiler and libraries first; merely copying `sqlite3.h` is not enough for password hashing. Do not add `sqlite3.c` to the build if you link against a compiled SQLite library. The simplest route for this project is the CMake build above.

## First run
The program creates `lms.db` in its working directory. Register users, then log in with one of the four roles. The first prototype intentionally exposes registration for development; add a secure first-run administrator setup and restrict subsequent user creation before production.

## Current prototype capabilities
- SQLite schema creation with foreign keys enabled.
- Four polymorphic user types and role-aware menu checks.
- Parameterised queries for user lookup/creation and LMS records.
- Salted password hashes via libsodium's password-hashing API (Argon2id).
- Assignment creation, submissions with attempt history, extensions, marking, moderation and approved mark release.
- Mark insert/update and corresponding audit event in one transaction.
- Exceptions for invalid operations and database errors.

## Important gaps to close before submission
1. Compare every function with the exact Phase 2 SRS Must requirements and add a traceability table.
2. Replace the default 10% per late day strategy with the precise SRS rule if different. The Phase 3 design says submissions 1–3 days late incur a penalty and submissions more than 3 days late receive zero; the exact percentage was not visible in the Phase 3 source used here.
3. Validate all role-to-entity relationships (e.g., lecturer assigned to course, student enrolled in course), effective deadlines, timestamp formats, and score <= assignment maximum in service layer.
4. Add a complete moderation audit history including moderator, time, decision and reason; currently `AuditLog` is specifically for mark changes.
5. Add database transaction and error tests; restrict account creation to authorised administrators; avoid showing released marks to anyone other than their owner.
6. Ensure extensions, late-penalty application, resubmission rules and mark release follow the SRS exactly.
7. Add automated unit/integration tests and a CMake test target for Phase 5.

## Coding standard
- Types/classes: `PascalCase`; functions/variables: `camelCase`; constants: `UPPER_SNAKE_CASE`.
- Use `const` where possible, initialise variables, use RAII, avoid raw ownership, validate external input, use parameterised SQL, and throw descriptive exceptions.
- Keep UI, business rules, and persistence in separate layers.

## Data / security warning
This is an implementation starter and must be tested and reconciled with your SRS before submission. Do not use real student data or real passwords while testing.
