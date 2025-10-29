#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <stdbool.h>

#define NAME_LENGTH 50
#define MIN_ID 1
#define MAX_ID 10000
#define MIN_PRICE 0.01
#define MAX_PRICE 100000.0

typedef struct
{
    int productID;
    char productName[NAME_LENGTH];
    float price;
    int quantity;
} Product;

int getValidID(char *message)
{
    int value;
    char extracharacter;
    while (1)
    {
        printf("%s", message);
        if (scanf("%d%c", &value, &extracharacter) != 2 || extracharacter != '\n')
        {
            printf("Invalid input! Enter an integer.\n");
            while (getchar() != '\n');
        }
        else if (value < MIN_ID || value > MAX_ID)
        {
            printf("Invalid ID! ID should be between %d and %d.\n", MIN_ID, MAX_ID);
        }
        else
        {
            return value;
        }
    }
}

float getValidPrice(char *message)
{
    float value;
    char extracharacter;
    while (1)
    {
        printf("%s", message);
        if (scanf("%f%c", &value, &extracharacter) != 2 || extracharacter != '\n')
        {
            printf("Invalid input! Enter a number.\n");
            while (getchar() != '\n');
        }
        else if (value < MIN_PRICE || value > MAX_PRICE)
        {
            printf("Invalid price! Price should be between %.2f and %.2f.\n", MIN_PRICE, MAX_PRICE);
        }
        else
        {
            return value;
        }
    }
}

void getValidName(char *message, char *string, int maxLength)
{
    while (1)
    {
        printf("%s", message);

        if (fgets(string, maxLength, stdin) == NULL)
        {
            printf("Error reading input! Try again.\n");
            continue;
        }

        string[strcspn(string, "\n")] = '\0';

        if (strlen(string) == 0)
        {
            printf("Invalid input! Name cannot be empty.\n");
            continue;
        }

        bool isValid = true;
        for (int i = 0; string[i] != '\0'; i++)
        {
            if (!isalpha((unsigned char)string[i]))
            {
                isValid = false;
                break;
            }
        }

        if (!isValid)
        {
            printf("Invalid input! Name must contain letters only.\n");
            continue;
        }

        break;
    }
}

void toLowerCase(char *string)
{
    for (int i = 0; string[i]; i++)
        string[i] = tolower(string[i]);
}

void printProduct(Product product)
{
    printf("Product ID: %d | Name: %s | Price: %.2f | Quantity: %d\n",
           product.productID, product.productName, product.price, product.quantity);
}

void viewProducts(Product *products, int count)
{
    if (count == 0)
    {
        printf("No products available.\n");
        return;
    }
    printf("========= PRODUCT LIST =========\n");
    for (int i = 0; i < count; i++)
        printProduct(products[i]);
}

void addProduct(Product **products, int *count)
{
    *products = (Product *)realloc(*products, (*count + 1) * sizeof(Product));
    if (*products == NULL)
    {
        printf("Memory allocation failed!\n");
        return;
    }

    int newID;
    char newName[NAME_LENGTH];

    while (1)
    {
        newID = getValidID("Product ID: ");
        bool idExists = false;
        for (int i = 0; i < *count; i++)
        {
            if ((*products)[i].productID == newID)
            {
                idExists = true;
                break;
            }
        }
        if (idExists)
            printf("Error! Product ID already exists. Enter a unique ID.\n");
        else
            break;
    }

    while (1)
    {
        getValidName("Product Name: ", newName, NAME_LENGTH);
        bool nameExists = false;
        for (int i = 0; i < *count; i++)
        {
            if (strcasecmp((*products)[i].productName, newName) == 0)
            {
                nameExists = true;
                break;
            }
        }
        if (nameExists)
            printf("Error! Product Name already exists. Enter a unique name.\n");
        else
            break;
    }

    (*products)[*count].productID = newID;
    strcpy((*products)[*count].productName, newName);
    (*products)[*count].price = getValidPrice("Product Price: ");
    (*products)[*count].quantity = getValidID("Product Quantity: ");

    (*count)++;
    printf("Product added successfully!\n");
}

void updateQuantity(Product *products, int count)
{
    int id = getValidID("Enter Product ID to update quantity: ");
    for (int i = 0; i < count; i++)
    {
        if (products[i].productID == id)
        {
            products[i].quantity = getValidID("Enter new Quantity: ");
            printf("Quantity updated successfully!\n");
            return;
        }
    }
    printf("Product with ID %d not found.\n", id);
}

void searchByID(Product *products, int count)
{
    int id = getValidID("Enter Product ID to search: ");
    for (int i = 0; i < count; i++)
    {
        if (products[i].productID == id)
        {
            printf("Product Found: ");
            printProduct(products[i]);
            return;
        }
    }
    printf("Product not found.\n");
}

void searchByName(Product *products, int count)
{
    char searchName[NAME_LENGTH];
    getValidName("Enter name to search (partial allowed): ", searchName, NAME_LENGTH);

    char lowerSearch[NAME_LENGTH];
    strcpy(lowerSearch, searchName);
    toLowerCase(lowerSearch);

    bool found = false;
    printf("Products Found:\n");
    for (int i = 0; i < count; i++)
    {
        char lowerProduct[NAME_LENGTH];
        strcpy(lowerProduct, products[i].productName);
        toLowerCase(lowerProduct);

        if (strstr(lowerProduct, lowerSearch) != NULL)
        {
            printProduct(products[i]);
            found = true;
        }
    }
    if (!found)
        printf("No matching products found.\n");
}

void searchByPriceRange(Product *products, int count)
{
    float minPrice = getValidPrice("Enter minimum price: ");
    float maxPrice = getValidPrice("Enter maximum price: ");

    bool found = false;
    printf("Products in price range:\n");
    for (int i = 0; i < count; i++)
    {
        if (products[i].price >= minPrice && products[i].price <= maxPrice)
        {
            printProduct(products[i]);
            found = true;
        }
    }
    if (!found)
        printf("No products found in this price range.\n");
}

void deleteProduct(Product **products, int *count)
{
    int id = getValidID("Enter Product ID to delete: ");
    bool found = false;

    for (int i = 0; i < *count; i++)
    {
        if ((*products)[i].productID == id)
        {
            for (int j = i; j < *count - 1; j++)
                (*products)[j] = (*products)[j + 1];

            *products = (Product *)realloc(*products, (*count - 1) * sizeof(Product));
            (*count)--;
            printf("Product deleted successfully!\n");
            found = true;
            break;
        }
    }

    if (!found)
        printf("Product with ID %d not found.\n", id);
}

int main()
{
    int initialCount = getValidID("Enter initial number of products: ");
    Product *inventory = (Product *)calloc(initialCount, sizeof(Product));
    if (inventory == NULL)
    {
        printf("Memory allocation failed!\n");
        return 1;
    }

    for (int i = 0; i < initialCount; i++)
    {
        printf("Enter details for product %d:\n", i + 1);

        int newID;
        char newName[NAME_LENGTH];

        while (1)
        {
            newID = getValidID("Product ID: ");
            bool idExists = false;
            for (int j = 0; j < i; j++)
            {
                if (inventory[j].productID == newID)
                {
                    idExists = true;
                    break;
                }
            }
            if (idExists)
                printf("Error! Product ID already exists. Enter a unique ID.\n");
            else
                break;
        }

        while (1)
        {
            getValidName("Product Name: ", newName, NAME_LENGTH);
            bool nameExists = false;
            for (int j = 0; j < i; j++)
            {
                if (strcasecmp(inventory[j].productName, newName) == 0)
                {
                    nameExists = true;
                    break;
                }
            }
            if (nameExists)
                printf("Error! Product Name already exists. Enter a unique name.\n");
            else
                break;
        }

        inventory[i].productID = newID;
        strcpy(inventory[i].productName, newName);
        inventory[i].price = getValidPrice("Product Price: ");
        inventory[i].quantity = getValidID("Product Quantity: ");
    }

    int choice;
    int productCount = initialCount;

    do
    {
        printf("\n========= INVENTORY MENU =========\n");
        printf("1. Add New Product\n");
        printf("2. View All Products\n");
        printf("3. Update Quantity\n");
        printf("4. Search Product by ID\n");
        printf("5. Search Product by Name\n");
        printf("6. Search Product by Price Range\n");
        printf("7. Delete Product\n");
        printf("8. Exit\n");

        choice = getValidID("Enter your choice: ");

        switch (choice)
        {
            case 1:
                addProduct(&inventory, &productCount);
                break;
            case 2:
                viewProducts(inventory, productCount);
                break;
            case 3:
                updateQuantity(inventory, productCount);
                break;
            case 4:
                searchByID(inventory, productCount);
                break;
            case 5:
                searchByName(inventory, productCount);
                break;
            case 6:
                searchByPriceRange(inventory, productCount);
                break;
            case 7:
                deleteProduct(&inventory, &productCount);
                break;
            case 8:
                free(inventory);
                printf("Memory released successfully. Exiting program...\n");
                break;
            default:
                printf("Invalid choice! Try again.\n");
        }

    } while (choice != 8);

    return 0;
}
