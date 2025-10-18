#include <stdio.h>

#define MAX_WORDS 100
#define MAX_LENGTH 50

int main() {
    int n;
    printf("Enter number of words: ");
    scanf("%d", &n);

    char words[MAX_WORDS][MAX_LENGTH];
    char (*ptr)[MAX_LENGTH] = words; 

    printf("Enter words:\n");
    for (int i = 0; i < n; i++) {
        scanf("%s", *(ptr + i)); 
    }

    char *longestWord = *(ptr + 0);
    int maxLength = 0;

    for (int i = 0; i < n; i++) {
        char *wordPtr = *(ptr + i);
        int length = 0;

        while (*(wordPtr + length) != '\0') {
            length++;
        }

        if (length > maxLength) {
            maxLength = length;
            longestWord = *(ptr + i);
        }
    }

    printf("The longest word is: %s\n", longestWord);

    return 0;
}
