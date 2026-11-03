# Phase 7: Release Notes

## 1. Release Information

| Item | Details |
|---|---|
| Project | Learning Management System (LMS) |
| Version | 1.0.0 |
| Release Type | Initial Release |
| Programming Language | C++17 |
| Database | SQLite |
| Build System | CMake and Ninja |
| Platform | Windows with MSYS2 UCRT64 |

## 2. Release Overview

Version 1.0.0 is the initial release of the Learning Management System prototype. The system provides a console-based application for managing assignments, student submissions, marking, deadline extensions, moderation and the release of approved marks.

The application uses SQLite for persistent data storage and libsodium for secure password hashing.

## 3. Implemented Features

The following features are included in this release:

- User access for Students, Lecturers, Heads of Department (HoDs) and Administrators.
- User authentication and password hashing using libsodium Argon2id.
- Assignment creation and listing.
- Student assignment submissions.
- Submission listing for authorised staff.
- Submission marking and feedback.
- Late-submission penalty handling.
- Individual student deadline extensions.
- Mark moderation and approval by the HoD.
- Release of approved marks.
- Students can view their own released marks.
- SQLite database persistence.
- Automated testing using Catch2 and CTest.

## 4. Technical Implementation

The application is developed using C++17 and the following technologies:

| Technology | Purpose |
|---|---|
| C++17 | Application implementation |
| SQLite | Persistent database storage |
| libsodium | Password hashing |
| CMake | Build configuration |
| Ninja | Build execution |
| Catch2 | Automated testing framework |
| CTest | Test execution |

Database operations are handled through the Data Access Layer (DAL). Parameterised queries are used for database operations.

## 5. Installation and Execution

The application requires the MSYS2 UCRT64 environment and the dependencies described in the installation guide.

Refer to `Phase7_Installation_Guide.md` for the complete installation and build instructions.

After building the application, run:

```bash
./build/lms.exe
```

The LMS runs as a console-based application.

## 6. Known Limitations

The current release has the following limitations:

- The application does not provide a graphical user interface.
- The database uses local SQLite storage rather than a central online database.
- Deadline extensions are granted to individual students only.
- Students manually enter submission timestamps.
- The executable depends on the required MSYS2 UCRT64 runtime libraries.
- The application is a prototype and is not packaged as a standalone installer.

## 7. Future Improvements

Possible improvements for future versions include:

- Allowing lecturers to extend deadlines for an entire class.
- Displaying extended deadlines on student assignment lists.
- Developing a graphical user interface.
- Supporting centralised database access.
- Providing a packaged installation process.

## 8. Release Status

Version 1.0.0 represents the initial LMS prototype release. The features and limitations described in this document reflect the current implementation.

Future changes should be tested, reviewed and documented before being included in a subsequent release.