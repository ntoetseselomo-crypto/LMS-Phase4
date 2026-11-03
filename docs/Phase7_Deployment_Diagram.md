# Phase 7: Deployment Diagram

## 1. Introduction

The Learning Management System (LMS) is a C++17 console application deployed on a Windows workstation. It uses SQLite for persistent data storage and libsodium for password hashing.

## 2. UML Deployment Diagram

```mermaid
flowchart TB
    USERS["LMS Users<br/>Student<br/>Lecturer<br/>Head of Department<br/>Administrator"]

    subgraph PC["Windows School Workstation"]
        subgraph APP["LMS Application Process"]
            EXE["LMS Executable<br/>lms.exe"]
            AUTH["Authentication Service<br/>libsodium / Argon2id"]
            SERVICE["LMS Services"]
            REPO["Repository Layer"]
            DAL["Database Layer"]
        end

        DB[("SQLite Database File<br/>.lms.db")]
        RUNTIME["MSYS2 UCRT64 Runtime<br/>C++ Runtime DLLs<br/>SQLite DLL<br/>libsodium DLL"]
    end

    USERS --> EXE
    EXE --> AUTH
    EXE --> SERVICE
    SERVICE --> REPO
    REPO --> DAL
    DAL --> DB
    EXE -.-> RUNTIME

## 3. Deployment Nodes

| Deployment node | Description |
|---|---|
| Windows workstation | Computer where the LMS is installed and executed. |
| LMS application process | Runs the compiled `lms.exe` application. |
| Authentication service | Uses libsodium for Argon2id password hashing. |
| Services and repository layer | Handles LMS operations and data access. |
| Database layer | Communicates with SQLite. |
| SQLite database file | Stores persistent LMS information. |
| Runtime environment | Provides the DLL dependencies required by the executable. |

## 4. Database Deployment

The LMS uses an SQLite database file. The application selects the database path in this order:

1. The `LMS_DB` environment variable, if configured.
2. The `.lms.db` file in the user's `HOME` directory, if `HOME` is configured.
3. `lms.db` in the current working directory otherwise.

The application opens or creates the database and initialises its schema when it starts.

## 5. Runtime Requirements

The current Windows executable depends on runtime libraries, including:

- GCC and C++ runtime libraries.
- SQLite runtime library.
- libsodium runtime library.
- POSIX threads compatibility library.

The matching MSYS2 UCRT64 runtime environment must be available when running the application.

## 6. Deployment Assumptions and Limitations

- The LMS is a console-based application.
- The application and SQLite database are deployed locally.
- The current implementation does not require a web server or cloud database.
- This diagram represents the current prototype deployment.
