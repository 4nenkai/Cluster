#ifndef DATABASE
#define DATABASE
#include "sqlite/sqlite3.h"
#include <iostream>

class DatabaseMgmt {
public:
    ~DatabaseMgmt()
    {
        std::cout << "Database Management Stack has been called." << '\n';
    }

    static void initialize_database(sqlite3*& db);
    static bool is_user_blacklisted(sqlite3* db, const std::string& userid);
    static void add_user_to_blacklist(sqlite3* db, const std::string& userid, const std::string& auth, std::string timestamp, std::string reason);
    static void remove_user_from_blacklist(sqlite3* db, const std::string& userid);
    static std::string get_data_by_column_name(sqlite3* db, const std::string& table_name, const std::string& column_name, const std::string& search_value, const std::string& target_column);
    static std::string get_blacklist_logs(sqlite3* db, std::string user_id);
    inline const static std::vector<std::string> client_administrator = { "1186263857585471528" };
};

void DatabaseMgmt::initialize_database(sqlite3*& db) {
    int rc = sqlite3_open("blacklist.db", &db);
    if (rc) {
        std::cerr << "Can't open database: " << sqlite3_errmsg(db) << std::endl;
        exit(1);
    }

    const char* sqls = "CREATE TABLE IF NOT EXISTS logs ("
                  "id INTEGER PRIMARY KEY AUTOINCREMENT, "
                  "user_id TEXT NOT NULL, "
                  "by TEXT NOT NULL, "
                  "reason TEXT NOT NULL, "
                  "timestamp TEXT NOT NULL"
                  ");";

    char* errMsgs = nullptr;
    int rcs = sqlite3_exec(db, sqls, nullptr, nullptr, &errMsgs);

    if (rcs != SQLITE_OK) {
        std::cerr << "SQL error: " << errMsgs << std::endl;
        sqlite3_free(errMsgs);
        exit(1);
    }

    const char* sql = "CREATE TABLE IF NOT EXISTS blacklist ("
        "userid TEXT PRIMARY KEY, "
        "auth TEXT NOT NULL, "
        "timestamp TEXT NOT NULL,"
        "reason TEXT"
        ");";
    char* errMsg = 0;
    rc = sqlite3_exec(db, sql, 0, 0, &errMsg);
    if (rc != SQLITE_OK) {
        std::cerr << "SQL error: " << errMsg << std::endl;
        sqlite3_free(errMsg);
        exit(1);
    }
}

std::string DatabaseMgmt::get_blacklist_logs(sqlite3* db, std::string user_id) {
    std::stringstream logs;

    std::string sql = "SELECT by, reason, timestamp FROM logs WHERE user_id = ?;";
    sqlite3_stmt* stmt;

    int rc = sqlite3_prepare_v2(db, sql.c_str(), -1, &stmt, nullptr);
    if (rc == SQLITE_OK) {
        rc = sqlite3_bind_text(stmt, 1, user_id.c_str(), -1, SQLITE_STATIC);
        while (sqlite3_step(stmt) == SQLITE_ROW) {
            std::string user_id = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 0));
            std::string reason = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 1));
            std::string timestamp = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 2));

            logs << "By: <@" << user_id << ">, Reason: " << reason << ", At: <t:" << timestamp << ">\n";
        }
        sqlite3_finalize(stmt);
    }
    else {
        std::cerr << "Failed to execute SQL: " << sqlite3_errmsg(db) << std::endl;
    }

    return logs.str();
}

std::string DatabaseMgmt::get_data_by_column_name(sqlite3* db, const std::string& table_name, const std::string& column_name, const std::string& search_value, const std::string& target_column) {
    std::string result;
    std::string sql = "SELECT " + target_column + " FROM " + table_name + " WHERE " + column_name + " = ?;";
    sqlite3_stmt* stmt;

    if (sqlite3_prepare_v2(db, sql.c_str(), -1, &stmt, nullptr) != SQLITE_OK) {
        std::cerr << "SQL error: " << sqlite3_errmsg(db) << std::endl;
        return result;
    }

    if (sqlite3_bind_text(stmt, 1, search_value.c_str(), -1, SQLITE_STATIC) != SQLITE_OK) {
        std::cerr << "SQL error: " << sqlite3_errmsg(db) << std::endl;
        sqlite3_finalize(stmt);
        return result;
    }

    if (sqlite3_step(stmt) == SQLITE_ROW) {
        const unsigned char* data = sqlite3_column_text(stmt, 0);
        if (data) {
            result = reinterpret_cast<const char*>(data);
        }
    }
    else {
        std::cerr << "No data found." << std::endl;
    }

    sqlite3_finalize(stmt);

    return result;
}

bool DatabaseMgmt::is_user_blacklisted(sqlite3* db, const std::string& userid) {
    std::string sql = "SELECT 1 FROM blacklist WHERE userid = ?;";
    sqlite3_stmt* stmt;
    int rc = sqlite3_prepare_v2(db, sql.c_str(), -1, &stmt, 0);
    if (rc != SQLITE_OK) {
        std::cerr << "Failed to fetch data: " << sqlite3_errmsg(db) << std::endl;
        return false;
    }
    sqlite3_bind_text(stmt, 1, userid.c_str(), -1, SQLITE_STATIC);
    rc = sqlite3_step(stmt);
    bool result = (rc == SQLITE_ROW);
    sqlite3_finalize(stmt);
    return result;
}

void DatabaseMgmt::add_user_to_blacklist(sqlite3* db, const std::string& userid, const std::string& auth, std::string timestamp, std::string reason) {
    std::string sql = "INSERT OR IGNORE INTO blacklist (userid, auth, timeline, reason) VALUES (?, ?, ?, ?);";
    sqlite3_stmt* stmt;
    int rc = sqlite3_prepare_v2(db, sql.c_str(), -1, &stmt, 0);
    if (rc != SQLITE_OK) {
        std::cerr << "Failed to insert data: " << sqlite3_errmsg(db) << std::endl;
        return;
    }
    sqlite3_bind_text(stmt, 1, userid.c_str(), -1, SQLITE_STATIC);
    sqlite3_bind_text(stmt, 2, auth.c_str(), -1, SQLITE_STATIC);
    sqlite3_bind_text(stmt, 3, timestamp.c_str(), -1, SQLITE_STATIC);
    sqlite3_bind_text(stmt, 4, reason.c_str(), -1, SQLITE_STATIC);
    rc = sqlite3_step(stmt);
    if (rc != SQLITE_DONE) {
        std::cerr << "Failed to insert data: " << sqlite3_errmsg(db) << std::endl;
    }
    sqlite3_finalize(stmt);

    sql = "INSERT INTO logs (user_id, by, reason, timestamp) VALUES (?, ?, ?, ?)";
    sqlite3_stmt* stmt_;
    rc = sqlite3_prepare_v2(db, sql.c_str(), -1, &stmt_, 0);
    sqlite3_bind_text(stmt_, 1, userid.c_str(), -1, SQLITE_STATIC);
    sqlite3_bind_text(stmt_, 2, auth.c_str(), -1, SQLITE_STATIC);
    sqlite3_bind_text(stmt_, 3, reason.c_str(), -1, SQLITE_STATIC);
    sqlite3_bind_text(stmt_, 4, timestamp.c_str(), -1, SQLITE_STATIC);
    rc = sqlite3_step(stmt_);
    if (rc != SQLITE_DONE) {
        std::cerr << "Failed to insert data: " << sqlite3_errmsg(db) << std::endl;
    }
    sqlite3_finalize(stmt_);
}

void DatabaseMgmt::remove_user_from_blacklist(sqlite3* db, const std::string& userid) {
    std::string sql = "DELETE FROM blacklist WHERE userid = ?;";
    sqlite3_stmt* stmt;
    int rc = sqlite3_prepare_v2(db, sql.c_str(), -1, &stmt, 0);
    if (rc != SQLITE_OK) {
        std::cerr << "Failed to delete data: " << sqlite3_errmsg(db) << std::endl;
        return;
    }
    sqlite3_bind_text(stmt, 1, userid.c_str(), -1, SQLITE_STATIC);
    rc = sqlite3_step(stmt);
    if (rc != SQLITE_DONE) {
        std::cerr << "Failed to delete data: " << sqlite3_errmsg(db) << std::endl;
    }
    sqlite3_finalize(stmt);
}

#endif