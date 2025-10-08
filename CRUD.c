#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#define FILE_NAME "users.txt"

struct User {
    int id;
    char name[50];
    int age;
};

int idExists(int id) {
    struct User existingUser;
    FILE *fp = fopen(FILE_NAME, "r");
    if (!fp) return 0;
    while (fscanf(fp, "%d %s %d", &existingUser.id, existingUser.name, &existingUser.age) == 3) 
    {
        if (existingUser.id == id) 
        {
            fclose(fp);
            return 1;
        }
    }
    fclose(fp);
    return 0;
}

int isValidName(char *name) 
{
    if (strlen(name) == 0) return 0;
    for (int i = 0; name[i] != '\0'; i++)
        {
          if (!isalpha(name[i]) && name[i] != ' ')
            return 0;
    }
    return 1;
}

void addUser() 
{
    struct User newUser;
    FILE *fp=fopen(FILE_NAME, "a");
    if (!fp) 
    {
        printf("Cannot open file\n");
        return;
    }

    printf("Enter ID: ");
    if (scanf("%d", &newUser.id) != 1)
    {
        printf("Invalid input. ID must be numeric.\n");
        while (getchar() != '\n');
        fclose(fp);
        return;
    }

    if (newUser.id <= 0)
    {
        printf("ID must be positive.\n");
        fclose(fp);
        return;
    }

    if (idExists(newUser.id))
    {
        printf("This ID already exists. Choose a different one.\n");
        fclose(fp);
        return;
    }

    printf("Enter Name: ");
    scanf(" %[^\n]", newUser.name); 
    if (!isValidName(newUser.name)) {
        printf("Invalid name. Only letters and spaces allowed.\n");
        fclose(fp);
        return;
    }

    printf("Enter Age: ");
    if (scanf("%d", &newUser.age) != 1)
    {
        printf("Invalid input. Age must be numeric.\n");
        while (getchar() != '\n');
        fclose(fp);
        return;
    }

    if (newUser.age < 0 || newUser.age > 120) 
    {
        printf("Age must be between 0 and 120.\n");
        fclose(fp);
        return;
    }

    fprintf(fp, "%d %s %d\n", newUser.id, newUser.name, newUser.age);
    fclose(fp);
    printf("User added!\n");
}

void showUsers() 
{
    struct User existingUser;
    FILE *fp=fopen(FILE_NAME, "r");
    if (!fp)
    {
        printf("No users found\n");
        return;
    }
    printf("\n--- Users ---\n");
    while (fscanf(fp, "%d %s %d", &existingUser.id, existingUser.name, &existingUser.age) == 3)
        {
        printf("ID:%d Name:%s Age:%d\n", existingUser.id, existingUser.name, existingUser.age);
    }
    fclose(fp);
}

void updateUser()
{
    struct User currentUser;
    int id, found = 0;
    FILE *fp = fopen(FILE_NAME, "r");
    FILE *temp = fopen("temp.txt", "w");
    if (!fp || !temp) 
    {
        printf("Cannot open file\n");
        return;
    }

    printf("Enter ID to update: ");
    scanf("%d", &id);

    while (fscanf(fp, "%d %s %d", &currentUser.id, currentUser.name, &currentUser.age) == 3) 
    {
        if (currentUser.id == id) 
        {
            printf("Enter new Name: ");
            scanf(" %[^\n]", currentUser.name);
            printf("Enter new Age: ");
            scanf("%d", &currentUser.age);
            found = 1;
        }
        fprintf(temp, "%d %s %d\n", currentUser.id, currentUser.name, currentUser.age);
    }

    fclose(fp);
    fclose(temp);
    remove(FILE_NAME);
    rename("temp.txt", FILE_NAME);

    if (found) printf("User updated!\n");
    else printf("User not found\n");
}

void deleteUser() {
    struct User currentUser;
    int id, found=0;
    FILE *fp=fopen(FILE_NAME, "r");
    FILE *temp=fopen("temp.txt", "w");
    if (!fp || !temp) {
        printf("Cannot open file\n");
        return;
    }

    printf("Enter ID to delete: ");
    scanf("%d", &id);

    while (fscanf(fp, "%d %s %d", &currentUser.id, currentUser.name, &currentUser.age) == 3)
        {
        if (currentUser.id == id) {
            found = 1;
            continue;
        }
        fprintf(temp, "%d %s %d\n", currentUser.id, currentUser.name, currentUser.age);
    }

    fclose(fp);
    fclose(temp);
    remove(FILE_NAME);
    rename("temp.txt", FILE_NAME);

    if (found) printf("User deleted!\n");
    else printf("User not found\n");
}

int main() {
    int choice;
    while (1) {
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
