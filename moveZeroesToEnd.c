#include <stdio.h>

int main() 
{
    int size;
    printf("Enter number of elements: ");
    scanf("%d", &size);

    int array[size];
    printf("Enter array elements: ");
    for (int index = 0; index < size; index++) 
    {
        scanf("%d", array + index); 
    }

    int position = 0; 
    for (int index = 0; index < size; index++) 
    {
        if (*(array + index) != 0) 
        {
            *(array + position) = *(array + index);
            position++;
        }
    }

    for (int index = position; index < size; index++) 
    {
        *(array + index) = 0;
    }

    printf("Array after moving zeros: ");
    for (int index = 0; index < size; index++) 
    {
        printf("%d ", *(array + index));
    }
    printf("\n");

    return 0;
}
