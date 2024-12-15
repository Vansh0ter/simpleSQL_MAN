import sqlite3
import re
from datetime import datetime

def create_connection():
    conn = sqlite3.connect('phonebook.db')
    return conn

def create_table(conn):
    cursor = conn.cursor()
    cursor.execute('''
        CREATE TABLE IF NOT EXISTS contacts (
            id INTEGER PRIMARY KEY AUTOINCREMENT,
            first_name TEXT NOT NULL,
            last_name TEXT NOT NULL,
            phone_number TEXT NOT NULL UNIQUE,
            birth_date TEXT,
            UNIQUE(first_name, last_name)
        )
    ''')
    conn.commit()

# Ввод данных с проверкой
def input_contact():
    while True:
        first_name = input("Specify name: ").strip().title()
        last_name = input("Specify surname: ").strip().title()
        
        if not re.match("^[A-Za-z0-9 ]+$", first_name) or not re.match("^[A-Za-z0-9 ]+$", last_name):
            print("Use only letters and numbers(special symbols such as '@#$%' are prohibited)")
            continue
        
        phone_number = input("Specify telephone number(11 digits): ").strip()
        if not re.match("^\d{11}$", phone_number):
            print("Number should include only 11 digits")
            continue
        
        if phone_number.startswith('+7'):
            phone_number = '8' + phone_number[2:]
        
        birth_date = input("Specify date of birth (day.month.year) or leave it blank: ").strip()
        if birth_date and not validate_date(birth_date):
            print("Incorrect date")
            continue
        
        return first_name, last_name, phone_number, birth_date

def validate_date(date_text):
    try:
        datetime.strptime(date_text, "%d.%m.%Y")
        return True
    except ValueError:
        return False

# Добавление новой записи
def add_contact(conn):
    first_name, last_name, phone_number, birth_date = input_contact()
    
    cursor = conn.cursor()
    try:
        cursor.execute('INSERT INTO contacts (first_name, last_name, phone_number, birth_date) VALUES (?, ?, ?, ?)',
                       (first_name, last_name, phone_number, birth_date))
        conn.commit()
        print("Record added successfully")
    except sqlite3.IntegrityError:
        print("Such a record already exists. Aborting...")

# Просмотр всех записей
def view_contacts(conn):
    cursor = conn.cursor()
    cursor.execute('SELECT * FROM contacts')
    rows = cursor.fetchall()
    
    if not rows:
        print("DB is empty.")
    else:
        for row in rows:
            print(f"Name: {row[1]}, Surname: {row[2]}, Phone number: {row[3]}, D.O.B: {row[4]}")

# Поиск записей
def search_contacts(conn):
    first_name = input("Specify name for search (or leave it blank): ").strip().title()
    last_name = input("Specify surname for search (or leave it blank): ").strip().title()

    cursor = conn.cursor()
    query = "SELECT * FROM contacts WHERE 1=1"
    
    params = []
    
    if first_name:
        query += " AND first_name=?"
        params.append(first_name)
        
    if last_name:
        query += " AND last_name=?"
        params.append(last_name)

    cursor.execute(query, params)
    rows = cursor.fetchall()

    if not rows:
        print("Record not found")
    else:
        for row in rows:
            print(f"Name: {row[1]}, Surname: {row[2]}, Phone number: {row[3]}, D.O.B: {row[4]}")

# Удаление записи
def delete_contact(conn):
    first_name = input("Specify name for deletion: ").strip().title()
    last_name = input("Specify surname for deletion: ").strip().title()

    cursor = conn.cursor()
    cursor.execute('DELETE FROM contacts WHERE first_name=? AND last_name=?', (first_name, last_name))
    
    if cursor.rowcount == 0:
        print("Record not found.")
    else:
        conn.commit()
        print("Record deleted successfully")

# Изменение записи
def edit_contact(conn):
    first_name = input("Specify name for editing: ").strip().title()
    last_name = input("Specify surname for editing: ").strip().title()

    cursor = conn.cursor()
    cursor.execute('SELECT * FROM contacts WHERE first_name=? AND last_name=?', (first_name, last_name))
    
    contact = cursor.fetchone()
    
    if not contact:
        print("Record not found.")
        return
    
    print(f"Current record - Name: {contact[1]}, Surname: {contact[2]}, Phone Number: {contact[3]}, D.O.B: {contact[4]}")
    
    new_first_name, new_last_name, new_phone_number, new_birth_date = input_contact()

    try:
        cursor.execute('UPDATE contacts SET first_name=?, last_name=?, phone_number=?, birth_date=? WHERE id=?',
                       (new_first_name, new_last_name, new_phone_number, new_birth_date, contact[0]))
        conn.commit()
        print("Record edited successfully")
    except sqlite3.IntegrityError:
        print("Editing error. Aborting...")

# Вывод возраста
def calculate_age(conn):
    first_name = input("Specify name for age calculation: ").strip().title()
    last_name = input("Specify surname for age calculation: ").strip().title()

    cursor = conn.cursor()
    cursor.execute('SELECT birth_date FROM contacts WHERE first_name=? AND last_name=?', (first_name, last_name))
    
    contact = cursor.fetchone()

    if not contact or not contact[0]:
        print("D.O.B not found")
        return
    
    birth_date = datetime.strptime(contact[0], "%d.%m.%Y")
    
    age = datetime.now().year - birth_date.year - ((datetime.now().month, datetime.now().day) < (birth_date.month, birth_date.day))
    
    print(f"Age of {first_name} {last_name}: {age} years.")

# Основной интерфейс программы
def display_menu():
    print("______________________________________________")
    print("Select an action:")
    print("1. Add a record")
    print("2. Display contacts") 
    print("3. Search") 
    print("4. Delete contact") 
    print("5. Edit contact")
    print("6. Calculate age")
    print("0. Exit") 



def main():
    conn = create_connection()
    
    create_table(conn)

    print("______________________________________________")

    

    while True:
        display_menu()
        command = int(input())
        match command:
            case 1:
                add_contact(conn)
            case 2:
                view_contacts(conn)
            case 3:
                search_contacts(conn)
            case 4:
                delete_contact(conn)
            case 5:
                edit_contact(conn)
            case 6:
                calculate_age(conn)
            case 0:
                break
            case _:
                print("Wrong command. Try again")
    conn.close()

if __name__ == "__main__":
    main()
