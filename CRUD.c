#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <stdbool.h>

#define FILE_NAME "users.txt"

struct User {
    int id;
    char name[50];
    int age;
};

bool idExists(int id) {
    struct User existingUser;
    FILE *usersFile = fopen(FILE_NAME, "r");
    if (!usersFile) 
        return false;
    while (fscanf(usersFile, "%d %s %d", &existingUser.id, existingUser.name, &existingUser.age) == 3) 
    {
        if (existingUser.id == id) 
        {
            fclose(usersFile);
            return true;
        }
    }
    fclose(usersFile);
    return false;
}

bool isValidName(char *name) 
{
    if (strlen(name) == 0) return false;
    for (int i = 0; name[i] != '\0'; i++) 
    {
        if (!isalpha(name[i]) && name[i] != ' ') 
            return false;
    }
    return true;
}

void addUser() 
{
    struct User newUser;
    FILE *usersFile = fopen(FILE_NAME, "a");
    if (!usersFile) 
    {
        printf("Cannot open file\n");
        return;
    }

    printf("Enter ID: ");
    if (scanf("%d", &newUser.id) != 1) 
    {
        printf("Invalid input. Enter a numeric ID.\n");
        while (getchar() != '\n');
        fclose(usersFile);
        return;
    }

    if (newUser.id <= 0) 
    {
        printf("Invalid input. Enter a numeric ID.\n");
        fclose(usersFile);
        return;
    }

    if (idExists(newUser.id)) 
    {
        printf("Error: A user with ID %d already exists. Please enter a unique ID.\n", newUser.id);
        fclose(usersFile);
        return;
    }

    printf("Enter Name: ");
    scanf(" %[^\n]", newUser.name);
    if (!isValidName(newUser.name)) 
    {
        printf("Invalid name. Name must contain only letters and spaces.\n");
        fclose(usersFile);
        return;
    }

    printf("Enter Age: ");
    if (scanf("%d", &newUser.age) != 1 || newUser.age < 0 || newUser.age > 100) 
    {
        printf("Age must be between 0 and 100.\n");
        while (getchar() != '\n');
        fclose(usersFile);
        return;
    }

    fprintf(usersFile, "%d %s %d\n", newUser.id, newUser.name, newUser.age);
    fclose(usersFile);
    printf("User added!\n");
}

void showUsers() 
{
    struct User existingUser;
    FILE *usersFile = fopen(FILE_NAME, "r");
    if (!usersFile) 
    {
        printf("No users found\n");
        return;
    }
    printf("\n--- Users ---\n");
    while (fscanf(usersFile, "%d %s %d", &existingUser.id, existingUser.name, &existingUser.age) == 3) 
    {
        printf("ID:%d Name:%s Age:%d\n", existingUser.id, existingUser.name, existingUser.age);
    }
    fclose(usersFile);
}

void updateUser() 
{
    struct User currentUser;
    int id, found = 0;
    FILE *usersFile = fopen(FILE_NAME, "r");
    FILE *tempFile = fopen("temp.txt", "w");
    if (!usersFile || !tempFile) 
    {
        printf("Cannot open file\n");
        if (usersFile) fclose(usersFile);
        if (tempFile) fclose(tempFile);
        return;
    }

    printf("Enter ID to update: ");
    scanf("%d", &id);

    while (fscanf(usersFile, "%d %s %d", &currentUser.id, currentUser.name, &currentUser.age) == 3) 
    {
        if (currentUser.id == id) 
        {
            printf("Enter new Name: ");
            scanf(" %[^\n]", currentUser.name);
            printf("Enter new Age: ");
            scanf("%d", &currentUser.age);
            found = 1;
        }
        fprintf(tempFile, "%d %s %d\n", currentUser.id, currentUser.name, currentUser.age);
    }

    fclose(usersFile);
    fclose(tempFile);

    /* replace original file with temp file, handle errors */
    if (remove(FILE_NAME) != 0) {
        /* cleanup temp if replacement cannot proceed */
        remove("temp.txt");
        printf("Error updating file\n");
        return;
    }
    if (rename("temp.txt", FILE_NAME) != 0) {
        /* if rename fails, attempt to restore (temp already removed above) */
        printf("Error updating file\n");
        return;
    }

    if (found) printf("User updated!\n");
    else printf("User not found\n");
}

void deleteUser() 
{
    struct User currentUser;
    int id, found = 0;
    FILE *usersFile = fopen(FILE_NAME, "r");
    FILE *tempFile = fopen("temp.txt", "w");
    if (!usersFile || !tempFile) 
    {
        printf("Cannot open file\n");
        if (usersFile) fclose(usersFile);
        if (tempFile) fclose(tempFile);
        return;
    }

    printf("Enter ID to delete: ");
    scanf("%d", &id);

    while (fscanf(usersFile, "%d %s %d", &currentUser.id, currentUser.name, &currentUser.age) == 3) 
    {
        if (currentUser.id == id) 
        {
            found = 1;
            continue;
        }
        fprintf(tempFile, "%d %s %d\n", currentUser.id, currentUser.name, currentUser.age);
    }

    fclose(usersFile);
    fclose(tempFile);

    /* replace original file with temp file, handle errors */
    if (remove(FILE_NAME) != 0) {
        remove("temp.txt");
        printf("Error updating file\n");
        return;
    }
    if (rename("temp.txt", FILE_NAME) != 0) {
        printf("Error updating file\n");
        return;
    }

    if (found) printf("User deleted!\n");
    else printf("User not found\n");
}

int main() 
{
    int choice;
    while (1) 
    {
        printf("\n--- Menu ---\n");
        printf("1.Add User\n2.Show Users\n3.Update User\n4.Delete User\n5.Exit\n");
        printf("Enter choice: ");
        scanf("%d", &choice);

        if (choice == 1) addUser();
        else if (choice == 2) showUsers();
        else if (choice == 3) updateUser();
        else if (choice == 4) deleteUser();
        else if (choice == 5) {
            printf("Exiting program...\n");
            break;
        } else printf("Invalid choice\n");
    }
    return 0;
}
