#include <stdio.h>

int main() {
    int n;
    printf("Enter number of elements: ");
    scanf("%d", &n);

    int arr[n];
    printf("Enter array elements: ");
    for (int i = 0; i < n; i++) {
        scanf("%d", arr + i); 
    }

    int pos = 0; 
    for (int i = 0; i < n; i++) {
        if (*(arr + i) != 0) {
            *(arr + pos) = *(arr + i);
            pos++;
        }
    }

    for (int i = pos; i < n; i++) {
        *(arr + i) = 0;
    }

    printf("Array after moving zeros: ");
    for (int i = 0; i < n; i++) {
        printf("%d ", *(arr + i));
    }
    printf("\n");

    return 0;
}
