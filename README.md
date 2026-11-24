# Address Book Management System in C

A menu-driven Address Book Management System developed using C programming. The project allows users to manage contact information through operations such as adding, searching, editing, deleting and sorting contacts. It also supports file handling to store contact details in a CSV file for persistent storage.

## Features

- **Create Contact:** Add new contacts with name, phone number and email address.
- **Search Contact:** Search for existing contacts using name, phone number or email.
- **Edit Contact:** Update the name, phone number or email address of an existing contact.
- **Delete Contact:** Remove selected contacts from the address book.
- **List Contacts:** Display contacts sorted alphabetically by name or by phone number or email.
- **Input Validation:** Validate names, phone numbers and email addresses before storing them.
- **Duplicate Prevention:** Prevent duplicate phone numbers and email addresses.
- **File Handling:** Store and retrieve contact information using a CSV file.

## Technologies Used

- C Programming
- Structures
- Pointers
- Arrays
- String Handling
- File Handling
- Standard C Libraries

## Project Structure

```text
AddressBook
│
├── contact.c       # Contact management operations
├── contact.h       # Contact structures and declarations
├── file.c          # File handling functions
├── file.h          # File handling declarations
├── main.c          # Contact data loading and saving
├── contacts.csv    # Stored contact information
└── README.md
```

## Input Validation

The application implements basic validation rules:

- Names must contain only letters and digits, with a maximum length of 49 characters.
- Phone numbers must contain exactly 10 digits.
- Email addresses must contain one '@' symbol and end with '.com'.
- Duplicate phone numbers and email addresses are not allowed.
- The address book supports a maximum of 100 contacts.

## Data Storage

Contact details are stored in a CSV file (`contacts.csv`) containing three fields:

- Name
- Phone Number
- Email Address

The application uses file handling operations to read existing contacts and save updated contact information.

## Learning Outcomes

Through this project, the following concepts are implemented:

- Managing structured data using C structures.
- Performing CRUD (Create, Read, Update and Delete) operations.
- Working with pointers and arrays.
- Implementing string comparison and searching.
- Sorting records using the `qsort()` function.
- Reading from and writing to CSV files.
- Implementing input validation and duplicate checking.

## Future Improvements

- Add support for more flexible email validation.
- Introduce dynamic memory allocation.
- Improve CSV parsing to support commas within fields.
- Add a graphical user interface.
- Implement contact sorting and filtering options.
