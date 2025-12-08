#include <stdio.h>

#define MAX_SIZE 100

void removeVowel(char *str) 
{
    char *read = str;
    char *write = str;

    while (*read != '\0') 
    {
        if (*read != 'a' && *read != 'e' && *read != 'i' && *read != 'o' && *read != 'u' &&
            *read != 'A' && *read != 'E' && *read != 'I' && *read != 'O' && *read != 'U') 
        {
            *write = *read;
            write++;
        }
        read++;
    }
    *write = '\0';
}

int main() 
{
    char string[MAX_SIZE];

    printf("Enter a string: ");
    fgets(string, MAX_SIZE, stdin);

    removeVowel(string);

    printf("String after removing vowels: %s", string);
    return 0;
}
