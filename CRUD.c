#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define FILE_NAME "users.txt"
struct User{
    int id;
    char name[50];
    int age;
};

void Create(){
    struct User u;
    FILE *fp=fopen(FILE_NAME, "a");
    if (fp == NULL) {
        printf("Cannot open file\n");
        return;
    }

      printf("Enter ID: ");
    scanf("%d", &u.id);

    printf("Enter Name: ");
    scanf("%s", u.name);

    printf("Enter Age: ");
    scanf("%d", &u.age);

    fprintf(fp, "%d %s %d\n", u.id, u.name, u.age);
    fclose(fp);
    printf("User added successfully!\n");
}

void Read(){
     struct User u;
    FILE *fp=fopen(FILE_NAME, "r");
    if (fp == NULL) {
        printf("No users found\n");
        return;
    }

    printf("\n--- User Records ---\n");
    while (fscanf(fp, "%d %s %d", &u.id, u.name, &u.age) == 3) 
    {
        printf("ID: %d | Name: %s | Age: %d\n", u.id, u.name, u.age);
    }

    fclose(fp);
}

void Update() {
    struct User u;
    int id, found = 0;

    FILE *fp=fopen(FILE_NAME, "r");
    FILE *temp=fopen("temp.txt", "w");
    if (fp == NULL || temp == NULL) 
    {
        printf("Cannot open file\n");
        return;
    }

    printf("Enter the ID of the user to update: ");
    scanf("%d", &id);

    while (fscanf(fp, "%d %s %d", &u.id, u.name, &u.age) == 3) 
    {
        if (u.id == id) {
            printf("Enter new Name: ");
            scanf("%s", u.name);
            printf("Enter new Age: ");
            scanf("%d", &u.age);
            found=1;
        }
        fprintf(temp, "%d %s %d\n", u.id, u.name, u.age);
    }

    fclose(fp);
    fclose(temp);

    remove(FILE_NAME);
    rename("temp.txt", FILE_NAME);

    if (found) 
    {
        printf("User updated successfully!\n");
    } else {
        printf("User with given ID not found.\n");
    }
}

void Delete()
{
    struct User u;
    int id, found = 0;

    FILE *fp = fopen(FILE_NAME, "r");
    FILE *temp = fopen("temp.txt", "w");
     if (fp == NULL || temp == NULL)
     {
        printf("Cannot open file\n");
        return;
    }

    printf("Enter the ID of the user to delete: ");
    scanf("%d", &id);

    while (fscanf(fp, "%d %s %d", &u.id, u.name, &u.age) == 3) 
    {
        if (u.id == id)
        {
            found = 1;
            continue;
        }
        fprintf(temp, "%d %s %d\n", u.id, u.name, u.age);
    }

    fclose(fp);
    fclose(temp);

    remove(FILE_NAME);
    rename("temp.txt", FILE_NAME);

    if (found) 
    {
        printf("User deleted successfully!\n");
    } else
    {
        printf("User with given ID not found.\n");
    }
}

int main() {
    int choice;

    while (1) {
        printf("\n=== User Management System ===\n");
        printf("1. Create\n");
        printf("2. Read\n");
        printf("3. Update\n");
        printf("4. Delete\n");
        printf("5. Exit\n");
        printf("Enter your choice: ");
        scanf("%d", &choice);

        if (choice == 1) {
            Create();
        } else if (choice == 2) {
            Read();
        } else if (choice == 3) {
            Update();
        } else if (choice == 4) {
            Delete();
        } else if (choice == 5) {
            printf("Exiting program...\n");
            break;
        } else {
            printf("Invalid choice, please try again.\n");
        }
    }

    return 0;
}
