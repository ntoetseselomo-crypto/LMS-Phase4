
cmake_minimum_required(VERSION 3.16)

project(LMS_Phase4 LANGUAGES C CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# SQLite
find_package(SQLite3 REQUIRED)

# libsodium
find_package(PkgConfig QUIET)

if(PkgConfig_FOUND)
    pkg_check_modules(SODIUM libsodium)
endif()

# Main LMS application
add_executable(lms
    src/main.cpp
    src/Database.cpp
    src/Repositories.cpp
    src/User.cpp
    src/Services.cpp
)

target_include_directories(lms PRIVATE include)

target_link_libraries(lms PRIVATE SQLite::SQLite3)

if(SODIUM_FOUND)
    target_include_directories(lms PRIVATE ${SODIUM_INCLUDE_DIRS})
    target_link_libraries(lms PRIVATE ${SODIUM_LIBRARIES})
    target_compile_definitions(lms PRIVATE LMS_USE_SODIUM=1)
else()
    message(FATAL_ERROR
        "libsodium is required for secure Argon2id password hashing."
    )
endif()

# Catch2 testing framework
include(FetchContent)

FetchContent_Declare(
    Catch2
    GIT_REPOSITORY https://github.com/catchorg/Catch2.git
    GIT_TAG v3.8.1
)

FetchContent_MakeAvailable(Catch2)

# Automated testing
enable_testing()

add_executable(lms_tests
    tests/test_penalty.cpp
    tests/test_repository.cpp
    tests/test_services.cpp
    src/Services.cpp
    src/User.cpp
    src/Repositories.cpp
    src/Database.cpp
)

target_include_directories(lms_tests PRIVATE include ${SODIUM_INCLUDE_DIRS})

target_link_libraries(lms_tests
    PRIVATE Catch2::Catch2WithMain SQLite::SQLite3 ${SODIUM_LIBRARIES}
)

target_compile_definitions(lms_tests PRIVATE LMS_USE_SODIUM=1)

# Code coverage for automated tests
if(CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
    target_compile_options(lms_tests PRIVATE
        --coverage
        -O0
        -g
    )

    target_link_options(lms_tests PRIVATE
        --coverage
    )
endif()

include(${Catch2_SOURCE_DIR}/extras/Catch.cmake)
catch_discover_tests(lms_tests)
