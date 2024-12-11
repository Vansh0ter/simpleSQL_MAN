## About
A simple SQL manipultion tool that uses SQLite3.
**NOTE**: You should install [sqlite3](https://github.com/sqlite/sqlite) package in order to build this program.
## Functionality
1. Inserting keys and values
2. Editing existing values
3. Deleting
4. Creating a backup
5. Restoring DB's from existing backups
6. Appending data into a table from .csv file
7. Displaying a table
## Complexity
Sqlite3 uses B- trees which are pretty famous for their favourable time and space complexity results. 
| Operation | Average | Worst |
| --- | --- | --- |
| Insert | O(log n) | O(log n) |
| Search | O(log n) | O(log n) |
| Deletion | O(log n) | O(log n) | 
| Space | O(n) | O(n) |

*pulled from wikipedia.org
## Building
```sh
g++ main.cxx -l sqlite3 -o main
```
