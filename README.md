### About
This is a simple SQL manipultion tool that uses SQLite3.
**NOTE**: You should install [sqlite3](https://github.com/sqlite/sqlite) package in order to build this program.
### Functionality
1. Inserting keys and values
2. Editing existing values
3. Deleting
4. Creating a backup
5. Restoring DB's from existing backups
6. Displaying a table
### Building
```sh
g++ main.cxx -l sqlite3 -o main
```
