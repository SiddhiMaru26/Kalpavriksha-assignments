#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

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

int validId(char *message)
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

float validPrice(char *message)
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

void validName(char *message, char *string, int maxLength)
{
    int hasLetter;

    while (1)
    {
        printf("%s", message);
        scanf(" %[^\n]", string);

        if (strlen(string) == 0 || strlen(string) >= maxLength)
        {
            printf("Invalid input! Name should contain only letters.\n");
            continue;
        }

        hasLetter = 0;
        for (int i = 0; string[i] != '\0'; i++)
        {
            if (isalpha(string[i]))
            {
                hasLetter = 1;
                break;
            }
        }

        if (!hasLetter)
        {
            printf("Invalid input! Name must contain at least one letter.\n");
        }
        else
        {
            break;
        }
    }
}

int validQuantity(char *message)
{
    return validId(message);
}

void toLowerCase(char *string)
{
    for (int i = 0; string[i]; i++)
    {
        string[i] = tolower(string[i]);
    }
}

void printProduct(Product p)
{
    printf("Product ID: %d | Name: %s | Price: %.2f | Quantity: %d\n",
           p.productID, p.productName, p.price, p.quantity);
}

void viewProducts(Product *products, int count)
{
    if (count == 0)
    {
        printf("No products available.\n");
        return;
    }

    printf("========= PRODUCT LIST =========\n");
    for (int index = 0; index < count; index++)
    {
        printProduct(products[index]);
    }
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
        newID = validId("Product ID: ");
        int idExists = 0;

        for (int i = 0; i < *count; i++)
        {
            if ((*products)[i].productID == newID)
            {
                idExists = 1;
                break;
            }
        }

        if (idExists)
        {
            printf("Error! Product ID already exists. Enter a unique ID.\n");
        }
        else
        {
            break;
        }
    }

    while (1)
    {
        validName("Product Name: ", newName, NAME_LENGTH);
        int nameExists = 0;

        for (int i = 0; i < *count; i++)
        {
            if (strcasecmp((*products)[i].productName, newName) == 0)
            {
                nameExists = 1;
                break;
            }
        }

        if (nameExists)
        {
            printf("Error! Product Name already exists. Enter a unique name.\n");
        }
        else
        {
            break;
        }
    }

    (*products)[*count].productID = newID;
    strcpy((*products)[*count].productName, newName);
    (*products)[*count].price = validPrice("Product Price: ");
    (*products)[*count].quantity = validQuantity("Product Quantity: ");

    (*count)++;
    printf("Product added successfully!\n");
}

void updateQuantity(Product *products, int count)
{
    int id = validId("Enter Product ID to update quantity: ");

    for (int index = 0; index < count; index++)
    {
        if (products[index].productID == id)
        {
            products[index].quantity = validQuantity("Enter new Quantity: ");
            printf("Quantity updated successfully!\n");
            return;
        }
    }

    printf("Product with ID %d not found.\n", id);
}

void searchByID(Product *products, int count)
{
    int id = validId("Enter Product ID to search: ");

    for (int index = 0; index < count; index++)
    {
        if (products[index].productID == id)
        {
            printf("Product Found: ");
            printProduct(products[index]);
            return;
        }
    }

    printf("Product not found.\n");
}

void searchByName(Product *products, int count)
{
    char searchName[NAME_LENGTH];
    int found = 0;

    validName("Enter name to search (partial allowed): ", searchName, NAME_LENGTH);

    char lowerSearch[NAME_LENGTH];
    strcpy(lowerSearch, searchName);
    toLowerCase(lowerSearch);

    printf("Products Found:\n");

    for (int index = 0; index < count; index++)
    {
        char lowerProduct[NAME_LENGTH];
        strcpy(lowerProduct, products[index].productName);
        toLowerCase(lowerProduct);

        if (strstr(lowerProduct, lowerSearch) != NULL)
        {
            printProduct(products[index]);
            found = 1;
        }
    }

    if (!found)
    {
        printf("No matching products found.\n");
    }
}

void searchByPriceRange(Product *products, int count)
{
    float minPrice = validPrice("Enter minimum price: ");
    float maxPrice = validPrice("Enter maximum price: ");
    int found = 0;

    printf("Products in price range:\n");

    for (int index = 0; index < count; index++)
    {
        if (products[index].price >= minPrice && products[index].price <= maxPrice)
        {
            printProduct(products[index]);
            found = 1;
        }
    }

    if (!found)
    {
        printf("No products found in this price range.\n");
    }
}

void deleteProduct(Product **products, int *count)
{
    int id = validId("Enter Product ID to delete: ");
    int found = 0;

    for (int index = 0; index < *count; index++)
    {
        if ((*products)[index].productID == id)
        {
            for (int j = index; j < *count - 1; j++)
            {
                (*products)[j] = (*products)[j + 1];
            }

            *products = (Product *)realloc(*products, (*count - 1) * sizeof(Product));
            (*count)--;

            printf("Product deleted successfully!\n");
            found = 1;
            break;
        }
    }

    if (!found)
    {
        printf("Product with ID %d not found.\n", id);
    }
}

int main()
{
    int initialCount = validId("Enter initial number of products: ");

    Product *inventory = (Product *)calloc(initialCount, sizeof(Product));
    if (inventory == NULL)
    {
        printf("Memory allocation failed!\n");
        return 1;
    }

    for (int index = 0; index < initialCount; index++)
    {
        printf("Enter details for product %d:\n", index + 1);
        inventory[index].productID = validId("Product ID: ");
        validName("Product Name: ", inventory[index].productName, NAME_LENGTH);
        inventory[index].price = validPrice("Product Price: ");
        inventory[index].quantity = validQuantity("Product Quantity: ");
    }

    int choice;
    int productCount = initialCount;

    do
    {
        printf("========= INVENTORY MENU =========\n");
        printf("1. Add New Product\n");
        printf("2. View All Products\n");
        printf("3. Update Quantity\n");
        printf("4. Search Product by ID\n");
        printf("5. Search Product by Name\n");
        printf("6. Search Product by Price Range\n");
        printf("7. Delete Product\n");
        printf("8. Exit\n");

        choice = validId("Enter your choice: ");

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
