#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>

typedef struct
{
    int ID;
    char Name[51];
    int Age;
} User;

void clear_stdin() {
    int c;
    while ((c = getchar()) != '\n' && c != EOF);
}

void strip_newline(char *s) {
    size_t L=strlen(s);
    if (L && s[L-1] == '\n') s[L-1] = '\0';
}

void Create() {
    User u1, u2;
    FILE *fp=fopen("users.txt", "a+");
    if (!fp) 
    { 
        perror("Cannot open file"); 
        return; 
    }

    printf("Enter ID: ");
    if(scanf("%d", &u1.ID) != 1) 
    { 
        clear_stdin(); 
        printf("Invalid ID\n"); 
        fclose(fp); 
        return; 
    }
    clear_stdin();

    printf("Enter Name: ");
    if(!fgets(u1.Name, sizeof(u1.Name), stdin)) 
    { 
        printf("Name error\n"); 
        fclose(fp); 
        return; 
    }
    strip_newline(u1.Name);

    printf("Enter Age: ");
    if(scanf("%d", &u1.Age) != 1) 
    { 
        clear_stdin(); 
        printf("Invalid Age\n"); 
        fclose(fp); 
        return; 
    }
    clear_stdin();

    // check duplicate ID
    bool exists=false;
    rewind(fp);
    char line[200];
    while(fgets(line, sizeof(line), fp)) {
        int id;
        sscanf(line, "%d|", &id);
        if(id == u1.ID) 
        { 
            exists=true; 
            break; 
        }
    }
    if(exists) 
    { 
        printf("Cannot create user, UNIQUE ID constraint\n"); 
        fclose(fp); 
        return; 
    }

    fprintf(fp, "%d|%s|%d\n", u1.ID, u1.Name, u1.Age);
    printf("User created successfully.\n");
    fclose(fp);
}

void Read() {
    FILE *fp=fopen("users.txt", "r");
    if(!fp) 
    { 
        printf("No records found\n"); 
        return; 
    }

    printf("\nAll Users:\n");
    char line[200];
    while(fgets(line, sizeof(line), fp)) {
        User u1;
        char *token=strtok(line, "|");
        if(!token) 
        continue;
        u1.ID=atoi(token);

        token=strtok(NULL, "|");
        if(!token) 
        continue;
        strcpy(u1.Name, token);

        token = strtok(NULL, "|");
        if(!token) 
        continue;
        u1.Age=atoi(token);

        strip_newline(u1.Name); // remove newline if present
        printf("%d %s %d\n", u1.ID, u1.Name, u1.Age);
    }
    fclose(fp);
}

void Update() {
    int targetID;
    printf("Enter ID to update: ");
    if(scanf("%d", &targetID) != 1) { clear_stdin(); printf("Invalid input\n"); return; }
    clear_stdin();

    FILE *fp=fopen("users.txt", "r");
    FILE *temp=fopen("temp.txt", "w");
    if(!fp || !temp) 
    { 
        perror("File error"); 
        if(fp) fclose(fp); 
        if(temp) fclose(temp); 
        return; 
    }


    bool found = false;
    char line[200];
    while(fgets(line, sizeof(line), fp)) {
        User u1;
        char *token = strtok(line, "|");
        if(!token) continue;
        u1.ID = atoi(token);

        token = strtok(NULL, "|");
        if(!token) continue;
        strcpy(u1.Name, token);

        token = strtok(NULL, "|");
        if(!token) continue;
        u1.Age = atoi(token);

        strip_newline(u1.Name);

        if(u1.ID == targetID) {
            found = true;
            int choice, newAge;
            char newName[51];

            printf("Update name? (1=yes,0=no): ");
            if(scanf("%d", &choice) != 1) 
            { 
                clear_stdin(); 
                choice = 0; 
            }
            clear_stdin();
            if(choice) 
            { 
                printf("Enter new Name: "); 
                fgets(newName, sizeof(newName), stdin); 
                strip_newline(newName); 
                strcpy(u1.Name, newName); 
            }

            printf("Update Age? (1=yes,0=no): ");
            if(scanf("%d", &choice) != 1) 
            { 
                clear_stdin(); 
                choice = 0; 
            }
            clear_stdin();
            if(choice) 
            { 
                printf("Enter new Age: "); 
                scanf("%d", &newAge); 
                clear_stdin(); 
                u1.Age = newAge; 
            }
        }
        fprintf(temp, "%d|%s|%d\n", u1.ID, u1.Name, u1.Age);
    }

    fclose(fp);
    fclose(temp);

    if(found) 
    { 
        remove("users.txt"); 
        rename("temp.txt", "users.txt"); 
        printf("Record updated successfully\n"); 
    }
    else 
    { 
        remove("temp.txt"); 
        printf("ID not found\n");
     }
}

void Delete() {
    int targetID;
    printf("Enter ID to delete: ");
    if(scanf("%d", &targetID) != 1) 
    { 
        clear_stdin(); 
        printf("Invalid input\n"); 
        return; 
    }
    clear_stdin();

    FILE *fp=fopen("users.txt", "r");
    FILE *temp=fopen("temp.txt", "w");
    if(!fp || !temp) 
    { 
        perror("File error"); 
        if(fp) fclose(fp); 
        if(temp) fclose(temp); 
        return; 
    }

    bool found=false;
    char line[200];
    while(fgets(line, sizeof(line), fp)) {
        User u1;
        char *token = strtok(line, "|");
        if(!token) continue;
        u1.ID = atoi(token);

        token = strtok(NULL, "|");
        if(!token) continue;
        strcpy(u1.Name, token);

        token = strtok(NULL, "|");
        if(!token) continue;
        u1.Age = atoi(token);

        strip_newline(u1.Name);

        if(u1.ID == targetID) { found = true; continue; }

        fprintf(temp, "%d|%s|%d\n", u1.ID, u1.Name, u1.Age);
    }

    fclose(fp);
    fclose(temp);

    if(found) 
    { 
        remove("users.txt"); 
        rename("temp.txt", "users.txt"); 
        printf("Record deleted successfully\n"); 
    }
    else 
    {  
        remove("temp.txt"); 
        printf("ID not found\n"); 
    }
}

int main() {
    int choice;
    while(1) {
        printf("\nUser CRUD Menu\n");
        printf("1. Create\n2. Read\n3. Update\n4. Delete\n5. Exit\nChoose option: ");
        if(scanf("%d", &choice) != 1) { clear_stdin(); printf("Invalid input\n"); continue; }
        clear_stdin();
        switch(choice) {
            case 1: Create(); break;
            case 2: Read(); break;
            case 3: Update(); break;
            case 4: Delete(); break;
            case 5: printf("Exiting...\n"); return 0;
            default: printf("Enter 1-5\n");
        }
    }
}
