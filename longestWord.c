#include <stdio.h>

#define MAX_WORDS 100
#define MAX_LENGTH 50

int main() 
{
    int size;
    printf("Enter number of words: ");
    scanf("%d", &size);

    char words[MAX_WORDS][MAX_LENGTH];
    char (*pointer)[MAX_LENGTH] = words; 

    printf("Enter words:\n");
    for (int index = 0; index< size; index++) 
    {
        scanf("%s", *(pointer + index)); 
    }

    char *longestWord = *(pointer + 0);
    int maxLength = 0;

    for (int index = 0; index < size; index++) 
    {
        char *wordPtr = *(pointer + index);
        int length = 0;

        while (*(wordPtr + length) != '\0') 
        {
            length++;
        }

        if (length > maxLength) 
        {
            maxLength = length;
            longestWord = *(pointer + index);
        }
    }

    printf("The longest word is: %s\n", longestWord);

    return 0;
}
