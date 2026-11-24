#include <stdio.h>
#include <string.h>
#include <strings.h>
#include "file.h"

void saveContactsToFile(AddressBook *addressBook)
{
    FILE *fptr = fopen("contacts.csv", "w");

    if (fptr == NULL)
    {
        printf("Error opening file\n");
        return;
    }

    for (int i = 0; i < addressBook->contactCount; i++)
    {
        fprintf(fptr, "%s,%s,%s\n",
                addressBook->contacts[i].name,
                addressBook->contacts[i].phone,
                addressBook->contacts[i].email);
    }

    fclose(fptr);
    printf("%d contact(s) saved successfully to contacts.csv\n", addressBook->contactCount);
}

void loadContactsFromFile(AddressBook *addressBook)
{
    FILE *fptr = fopen("contacts.csv", "r");
    char line[200];

    addressBook->contactCount = 0;

    if (fptr == NULL)  
        return;

    while (addressBook->contactCount < MAX_CONTACTS && fgets(line, sizeof(line), fptr))
    {
        Contact c;

        line[strcspn(line, "\r\n")] = '\0';
        if (line[0] == '\0')
            continue;

        if (sscanf(line, "%49[^,],%19[^,],%49[^,]", c.name, c.phone, c.email) != 3)
            continue;   /* malformed line */

        if (strcasecmp(c.name, "name") == 0 && strcasecmp(c.phone, "phone") == 0)
            continue;

        addressBook->contacts[addressBook->contactCount++] = c;
    }

    fclose(fptr);
}