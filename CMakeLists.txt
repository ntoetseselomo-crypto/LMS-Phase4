cmake_minimum_required(VERSION 3.16)
project(LMS_Phase4 LANGUAGES C CXX)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

find_package(SQLite3 REQUIRED)
find_package(PkgConfig QUIET)
if(PkgConfig_FOUND)
  pkg_check_modules(SODIUM libsodium)
endif()

add_executable(lms
  src/main.cpp
  src/Database.cpp
  src/Repositories.cpp
  src/User.cpp
  src/Repositories.cpp
  src/Services.cpp
)
target_include_directories(lms PRIVATE include)
target_link_libraries(lms PRIVATE SQLite::SQLite3)

if(SODIUM_FOUND)
  target_include_directories(lms PRIVATE ${SODIUM_INCLUDE_DIRS})
  target_link_libraries(lms PRIVATE ${SODIUM_LIBRARIES})
  target_compile_definitions(lms PRIVATE LMS_USE_SODIUM=1)
else()
  message(FATAL_ERROR "libsodium is required for secure Argon2id password hashing. Install libsodium and its development files.")
endif()
