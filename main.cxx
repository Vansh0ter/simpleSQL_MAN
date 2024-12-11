#include <iostream>
#include <sqlite3.h>
#include <string>
#include <fstream>

class Database {
public:
    Database(const std::string& dbName) {
        if (sqlite3_open(dbName.c_str(), &db) != SQLITE_OK) {
            std::cerr << "Open DB error: " << sqlite3_errmsg(db) << std::endl;
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
	
	void displayAllRecords() {
        const char* sql = "SELECT * FROM records;";
        sqlite3_stmt* stmt;

        if (sqlite3_prepare_v2(db, sql, -1, &stmt, nullptr) == SQLITE_OK) {
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
	
    void searchRecord(const std::string& fieldName, const std::string& value) {
		// If you're looking for an algorithmic notation here, note that sqllite has its own search algorithms... Check their documentation for more.
		// Sqlite3 uses B-Trees within its backend. So basically all algorithms in worst cases are O(log n)
		// Their B-Trees "algoes" are too hard for me. So i may not understand all formalities to full extent.
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
		// I doubt, this is "the right way" to do backups. Real pro's should stick with SQLITE3API backup tools instead
        std::ifstream source("database.db", std::ios::binary);
        std::ofstream dest(backupFileName, std::ios::binary);
        dest << source.rdbuf();
    }

    void restoreDatabase(const std::string& backupFileName) {
        std::ifstream source(backupFileName, std::ios::binary);
        std::ofstream dest("database.db", std::ios::binary);
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

void displayMenu() 
{
	std::cout << "______________________________________________" << std::endl; // A separator
	std::cout << "Select an action:" << std::endl;
    std::cout << "1. Add a record" << std::endl;
    std::cout << "2. Delete record" << std::endl;
    std::cout << "3. Search" << std::endl;
    std::cout << "4. Edit an exsiting record" << std::endl;
    std::cout << "5. Create a backup" << std::endl;
    std::cout << "6. Restore DB from backup" << std::endl;
    std::cout << "7. Display DB" << std::endl;
    std::cout << "8. Clear Screen" << std::endl; // Dumb solution
    std::cout << "0. Exit" << std::endl;
}

void clearScreen() { 
	// Unified version of cls(or clear) that works on windows and unix simultaneously.
	std::cout << "\033[2J\033[1;1H";
}
int main() {
    Database db("database.db");
    db.createTable();
    
    int choice;
    while (true) {
        displayMenu();
        std::cin >> choice;
        std::cout << "______________________________________________" << std::endl;

        switch (choice) {
            case 1: { // Add
                std::string name, value;
                std::cout << "Specify a key: ";
                std::cin >> name;
                std::cout << "Specify a value: ";
                std::cin >> value;
                db.addRecord(name, value);
                break;
            }
            case 2: { // Delete
                std::string fieldName, value;
                std::cout << "Search by key or by value ? ";
                std::cin >> fieldName;
                std::cout << "Specify value: ";
                std::cin >> value;
                db.deleteRecord(fieldName, value);
                break;
            }
            case 3: { // Search
                std::string fieldName, value;
                std::cout << "Search by key or by value ? ";
                std::cin >> fieldName;
                std::cout << "Specify value: ";
                std::cin >> value;
                db.searchRecord(fieldName, value);
                break;
            }
            case 4: { // Edit
                int id;
                std::string newValue;
                std::cout << "Specify ID: ";
                std::cin >> id;
                std::cout << "Specify new value: ";
                std::cin >> newValue;
                db.editRecord(id, newValue);
                break;
            }
            case 5: { // Backup init
                db.backupDatabase("backup.db");
                std::cout << "Backup created" << std::endl;
                break;
            }
            case 6: { // Restoring from backup
                db.restoreDatabase("backup.db");
                std::cout << "DB restored from backup successfully." << std::endl;
                break;
            }
            case 7: { // Displaying all records 
				db.displayAllRecords();
				break;
			}
			case 8: {
				clearScreen();
				break;
			}
            case 0: // Exit
                return 0;
            default: // Misinput exception
                std::cout << "Wrong input, try again" << std::endl;
        }
    }
    
    return 0;
}
