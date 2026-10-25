#pragma once
#include <sqlite3.h>
#include <stdexcept>
#include <string>

class Database {
public:
    explicit Database(const std::string& path);
    ~Database();
    Database(const Database&) = delete;
    Database& operator=(const Database&) = delete;
    sqlite3* handle() const noexcept { return db_; }
    void execute(const std::string& sql);
private:
    sqlite3* db_{nullptr};
};
