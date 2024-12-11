#include <iostream>
#include <sqlite3.h>
#include <string>
#include <fstream>

class Database {
public:
    Database(const std::string& dbName) {
        if (sqlite3_open(dbName.c_str(), &db) != SQLITE_OK) {
            std::cerr << "open DB error: " << sqlite3_errmsg(db) << std::endl;
        }
    }

    ~Database() {
        sqlite3_close(db);
    }

    void createTable() {
        const char* sql = "CREATE TABLE IF NOT EXISTS records (id INTEGER PRIMARY KEY, name TEXT UNIQUE, value TEXT);";
        executeSQL(sql);
    }

    void addRecord(const std::string& name, const std::string& value) {
        std::string sql = "INSERT INTO records (name, value) VALUES ('" + name + "', '" + value + "');";
        executeSQL(sql.c_str());
    }

    void deleteRecord(const std::string& fieldName, const std::string& value) {
        std::string sql = "DELETE FROM records WHERE " + fieldName + " = '" + value + "';";
        executeSQL(sql.c_str());
    }

    void searchRecord(const std::string& fieldName, const std::string& value) {
        std::string sql = "SELECT * FROM records WHERE " + fieldName + " = '" + value + "';";
        sqlite3_stmt* stmt;
        
        if (sqlite3_prepare_v2(db, sql.c_str(), -1, &stmt, nullptr) == SQLITE_OK) {
            while (sqlite3_step(stmt) == SQLITE_ROW) {
                int id = sqlite3_column_int(stmt, 0);
                const char* name = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 1));
                const char* val = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 2));
                std::cout << "ID: " << id << ", Name: " << name << ", Value: " << val << std::endl;
            }
            sqlite3_finalize(stmt);
        } else {
            std::cerr << "Query error: " << sqlite3_errmsg(db) << std::endl;
        }
    }

    void editRecord(int id, const std::string& newValue) {
        std::string sql = "UPDATE records SET value = '" + newValue + "' WHERE id = " + std::to_string(id) + ";";
        executeSQL(sql.c_str());
    }

    void backupDatabase(const std::string& backupFileName) {
        // This is a simple copy operation; for a real backup, consider using SQLite's backup API.
        std::ifstream source("my_cosy_database.db", std::ios::binary);
        std::ofstream dest(backupFileName, std::ios::binary);
        dest << source.rdbuf();
    }

    void restoreDatabase(const std::string& backupFileName) {
        // This is a simple copy operation; for a real restore, consider using SQLite's backup API.
        std::ifstream source(backupFileName, std::ios::binary);
        std::ofstream dest("my_cosy_database.db", std::ios::binary);
        dest << source.rdbuf();
    }

private:
    sqlite3* db;

    void executeSQL(const char* sql) {
        char* errMsg;
        if (sqlite3_exec(db, sql, nullptr, nullptr, &errMsg) != SQLITE_OK) {
            std::cerr << "SQL error : " << errMsg << std::endl;
            sqlite3_free(errMsg);
        }
    }
};

int main() {
    Database db("my_cosy_database.db");
    
    db.createTable();
    
    db.addRecord("test1", "value1");
    db.searchRecord("name", "test1");
    
    db.editRecord(1, "new_value1");
    
    db.searchRecord("name", "test1");
    
    db.deleteRecord("name", "test1");
    
    db.backupDatabase("backup.db");
    
    return 0;
}
