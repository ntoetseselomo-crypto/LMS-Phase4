# Phase 7: LMS Installation Guide

## 1. Purpose

This guide explains how a school technician can install, build, configure and run the Learning Management System (LMS) on a Windows computer.

## 2. System Requirements

The following software and dependencies are required:

| Requirement | Purpose |
|---|---|
| Windows computer | Runs the LMS application. |
| MSYS2 UCRT64 | Provides the development and runtime environment. |
| GCC compiler | Compiles the C++17 source code. |
| CMake 3.16 or later | Configures the project build. |
| Ninja | Build system used by CMake. |
| SQLite3 | Provides database functionality. |
| libsodium | Provides secure password hashing. |
| Internet connection | Required during initial configuration to download Catch2 for testing. |

## 3. Installing Dependencies

1. Install MSYS2 on the Windows computer.
2. Open the MSYS2 UCRT64 terminal.
3. Update the package database and installed packages:

   ```bash
   pacman -Syu
4. If instructed to close the terminal, close it, reopen the UCRT64 terminal and complete any remaining updates.
5. Install the required packages:

   ```bash
   pacman -S --needed mingw-w64-ucrt-x86_64-gcc mingw-w64-ucrt-x86_64-cmake mingw-w64-ucrt-x86_64-ninja mingw-w64-ucrt-x86_64-sqlite3 mingw-w64-ucrt-x86_64-libsodium mingw-w64-ucrt-x86_64-pkgconf
   ```

## 4. Obtaining the LMS Source Code

Obtain the LMS project source code from the authorised project repository or copy the project folder to the school computer.

Open the UCRT64 terminal and navigate to the project directory.

Example:

```bash
cd /c/Users/user/Pictures/LMS
```

Use the actual directory where the project has been installed on the school computer.

## 5. Configuring and Building the Application

From the project root directory, configure the project:

```bash
cmake -S . -B build -G Ninja
```

Build the application and test executable:

```bash
cmake --build build
```

The first CMake configuration requires an internet connection to retrieve Catch2 if it is not already cached.

## 6. Running the LMS

Launch the application:

```bash
./build/lms.exe
```

The LMS starts in the terminal and displays its console menu.

The application initialises the SQLite database schema when it starts.

## 7. Database Configuration

The database location is selected in this order:

1. The `LMS_DB` environment variable.
2. `.lms.db` inside the user's `HOME` directory, if configured.
3. `lms.db` in the current working directory otherwise.

Ensure that the application has permission to create and write to the selected database location.

## 8. Verifying the Installation

Run the automated tests:

```bash
ctest --test-dir build --output-on-failure
```

A successful test run should report that all registered tests passed.

Also launch the application and confirm that the console menu appears.

## 9. Database Backup and Maintenance

- Close the LMS before copying its database file for backup.
- Keep backups in a secure location accessible only to authorised personnel.
- Do not delete or replace the active database during normal operation.
- Verify that a backup exists before performing maintenance.
- Protect database backups because they may contain user and academic records.

## 10. Troubleshooting

| Problem | Possible cause | Recommended action |
|---|---|---|
| `cmake: command not found` | CMake is not installed or UCRT64 is not being used. | Install CMake and reopen the UCRT64 terminal. |
| Compiler not found | GCC is missing or unavailable on PATH. | Install the GCC UCRT64 package. |
| SQLite header not found | SQLite development package is missing. | Install the SQLite UCRT64 package. |
| libsodium not found | libsodium development package is missing. | Install the libsodium UCRT64 package. |
| Catch2 download fails | Internet connection or download problem. | Check the connection and rerun CMake configuration. |
| Application cannot open database | Invalid database path or insufficient permissions. | Check `LMS_DB`, `HOME` and directory permissions. |
| Required DLL is missing | Runtime dependency is unavailable. | Run from the matching MSYS2 UCRT64 environment. |

## 11. Security and Operational Notes

- Use authorised accounts when accessing the LMS.
- Do not share passwords between users.
- Restrict access to the database and its backups.
- Keep the application and its dependencies maintained.
- Do not use real student records for testing without appropriate authorisation.

## 12. Installation Completion

The installation is considered complete when:

- All required dependencies are installed.
- CMake configuration succeeds.
- The application builds successfully.
- Automated tests pass.
- The LMS launches and displays its console menu.
- The database is created or opened successfully.
