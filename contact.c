#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>
#include <ctype.h>
#include "contact.h"
#include "file.h"

#define BUF 128

#define BY_NAME  1
#define BY_PHONE 2
#define BY_EMAIL 3


static void clearLine(void)
{
    int c;
    while ((c = getchar()) != '\n' && c != EOF);
}


static void readString(const char *prompt, char *buf)
{
    printf("%s", prompt);
    if (scanf(" %127[^\n]", buf) != 1)
        exit(EXIT_SUCCESS);
    clearLine();
}

static int readInt(const char *prompt)
{
    char buf[BUF], *end;
    long v;

    readString(prompt, buf);
    v = strtol(buf, &end, 10);
    if (*end != '\0' || v < 0 || v > 100000)
        return -1;
    return (int)v;
}



static int validName(const char *s)
{
    size_t n = strlen(s);
    if (n < 1 || n > 49)
        return 0;
    for (size_t i = 0; i < n; i++)
        if (!isalnum((unsigned char)s[i]))
            return 0;
    return 1;
}

static int validPhone(const char *s)
{
    if (strlen(s) != 10)
        return 0;
    for (int i = 0; i < 10; i++)
        if (!isdigit((unsigned char)s[i]))
            return 0;
    return 1;
}

static int validEmail(const char *s)
{
    size_t n = strlen(s);
    int atCount = 0;
    const char *at, *dot;

    if (n < 6 || n > 49)                      
        return 0;
    if (!isalnum((unsigned char)s[0]))         
        return 0;

    for (size_t i = 0; i < n; i++)
    {
        if (isupper((unsigned char)s[i]) || isspace((unsigned char)s[i]))
            return 0;                         
        if (s[i] == '@')
            atCount++;
    }
    if (atCount != 1)                          
        return 0;

    dot = s + n - 4;
    if (strcmp(dot, ".com") != 0)              
        return 0;

    at = strchr(s, '@');
    if (dot - at < 2)                          
        return 0;

    return 1;
}


static int phoneExists(AddressBook *ab, const char *phone, int skip)
{
    for (int i = 0; i < ab->contactCount; i++)
        if (i != skip && strcmp(ab->contacts[i].phone, phone) == 0)
            return 1;
    return 0;
}

static int emailExists(AddressBook *ab, const char *email, int skip)
{
    for (int i = 0; i < ab->contactCount; i++)
        if (i != skip && strcmp(ab->contacts[i].email, email) == 0)
            return 1;
    return 0;
}


static void promptName(char *out)
{
    char buf[BUF];
    while (1)
    {
        readString("Enter name: ", buf);
        if (validName(buf))
        {
            strcpy(out, buf);
            return;
        }
        printf("Invalid name. Use only letters and digits (max 49 characters).\n");
    }
}

static void promptPhone(AddressBook *ab, char *out, int skip)
{
    char buf[BUF];
    while (1)
    {
        readString("Enter phone (10 digits): ", buf);
        if (!validPhone(buf))
            printf("Invalid phone. It must be exactly 10 digits.\n");
        else if (phoneExists(ab, buf, skip))
            printf("This phone number already exists. Enter a different one.\n");
        else
        {
            strcpy(out, buf);
            return;
        }
    }
}

static void promptEmail(AddressBook *ab, char *out, int skip)
{
    char buf[BUF];
    while (1)
    {
        readString("Enter email: ", buf);
        if (!validEmail(buf))
            printf("Invalid email. Needs one '@', ends with .com, at least one char between '@' and .com,\n"
                   "no uppercase letters, and must start with a letter or digit.\n");
        else if (emailExists(ab, buf, skip))
            printf("This email already exists. Enter a different one.\n");
        else
        {
            strcpy(out, buf);
            return;
        }
    }
}


static void printHeader(void)
{
    printf("\n%-5s %-20s %-12s %-30s\n", "S.No", "Name", "Phone", "Email");
}

static void printRow(int serial, const Contact *c)
{
    printf("%-5d %-20s %-12s %-30s\n", serial, c->name, c->phone, c->email);
}

static int selectField(const char *action)
{
    int n;
    printf("\n%s based on:\n1. Name\n2. Phone\n3. Email\n", action);
    n = readInt("Enter choice: ");
    if (n < BY_NAME || n > BY_EMAIL)
    {
        printf("Invalid choice.\n");
        return 0;
    }
    return n;
}

static int findMatches(AddressBook *ab, int field, const char *query, int *matches)
{
    int count = 0;
    for (int i = 0; i < ab->contactCount; i++)
    {
        const char *target = (field == BY_NAME)  ? ab->contacts[i].name
                           : (field == BY_PHONE) ? ab->contacts[i].phone
                                                 : ab->contacts[i].email;
        if (strcasestr(target, query) != NULL)
            matches[count++] = i;
    }
    return count;
}


static int searchAndList(AddressBook *ab, const char *action, int *matches)
{
    char query[BUF];
    int field = selectField(action);
    int count;

    if (field == 0)
        return 0;

    readString(field == BY_NAME  ? "Enter name to search: "
             : field == BY_PHONE ? "Enter phone to search: "
                                 : "Enter email to search: ", query);

    count = findMatches(ab, field, query, matches);
    if (count == 0)
    {
        printf("No matching contacts found.\n");
        return 0;
    }

    printHeader();
    for (int i = 0; i < count; i++)
        printRow(i + 1, &ab->contacts[matches[i]]);
    return count;
}

static int pickSerial(const int *matches, int count, const char *action)
{
    char prompt[64];
    int s;

    snprintf(prompt, sizeof(prompt), "Enter serial number to %s (0 to cancel): ", action);
    s = readInt(prompt);
    if (s == 0)
    {
        printf("Cancelled.\n");
        return -1;
    }
    if (s < 1 || s > count)
    {
        printf("Invalid serial number.\n");
        return -1;
    }
    return matches[s - 1];
}



static int cmpName(const void *a, const void *b)
{
    return strcasecmp(((const Contact *)a)->name, ((const Contact *)b)->name);
}
static int cmpPhone(const void *a, const void *b)
{
    return strcmp(((const Contact *)a)->phone, ((const Contact *)b)->phone);
}
static int cmpEmail(const void *a, const void *b)
{
    return strcmp(((const Contact *)a)->email, ((const Contact *)b)->email);
}



void initialize(AddressBook *addressBook)
{
    addressBook->contactCount = 0;
    loadContactsFromFile(addressBook);   
}

void createContact(AddressBook *addressBook)
{
    Contact c;

    if (addressBook->contactCount >= MAX_CONTACTS)
    {
        printf("Address book is full (%d contacts).\n", MAX_CONTACTS);
        return;
    }

    promptName(c.name);
    promptPhone(addressBook, c.phone, -1);
    promptEmail(addressBook, c.email, -1);

    addressBook->contacts[addressBook->contactCount++] = c;
    printf("Contact created successfully.\n");
}

void listContacts(AddressBook *addressBook)
{
    Contact sorted[MAX_CONTACTS];
    int criteria;

    if (addressBook->contactCount == 0)
    {
        printf("No contacts to display.\n");
        return;
    }

    criteria = selectField("Sort");
    if (criteria == 0)
        return;

    memcpy(sorted, addressBook->contacts, addressBook->contactCount * sizeof(Contact));
    qsort(sorted, addressBook->contactCount, sizeof(Contact),
          criteria == BY_NAME ? cmpName : criteria == BY_PHONE ? cmpPhone : cmpEmail);

    printHeader();
    for (int i = 0; i < addressBook->contactCount; i++)
        printRow(i + 1, &sorted[i]);
}

void searchContact(AddressBook *addressBook)
{
    int matches[MAX_CONTACTS];
    searchAndList(addressBook, "Search", matches);
}

void deleteContact(AddressBook *addressBook)
{
    int matches[MAX_CONTACTS];
    int count = searchAndList(addressBook, "Delete", matches);
    int index;

    if (count == 0)
        return;

    index = pickSerial(matches, count, "delete");
    if (index < 0)
        return;

    for (int i = index; i < addressBook->contactCount - 1; i++)
        addressBook->contacts[i] = addressBook->contacts[i + 1];
    addressBook->contactCount--;

    printf("Contact deleted successfully.\n");
}

void editContact(AddressBook *addressBook)
{
    int matches[MAX_CONTACTS];
    int count = searchAndList(addressBook, "Edit", matches);
    int index, field;
    Contact *c;

    if (count == 0)
        return;

    index = pickSerial(matches, count, "edit");
    if (index < 0)
        return;
    c = &addressBook->contacts[index];

    printf("\nWhich field do you want to edit?\n1. Name\n2. Phone\n3. Email\n");
    field = readInt("Enter choice: ");

    switch (field)
    {
        case BY_NAME:
            promptName(c->name);
            printf("Name updated successfully.\n");
            break;
        case BY_PHONE:
            promptPhone(addressBook, c->phone, index);
            printf("Phone updated successfully.\n");
            break;
        case BY_EMAIL:
            promptEmail(addressBook, c->email, index);
            printf("Email updated successfully.\n");
            break;
        default:
            printf("Invalid choice.\n");
    }
}

void saveAndExit(AddressBook *addressBook)
{
    saveContactsToFile(addressBook);
    exit(EXIT_SUCCESS);
}

   printf("Loaded %d contact(s)\n", addressBook->contactCount);