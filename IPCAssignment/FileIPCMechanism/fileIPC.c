#include <stdio.h>
#include <stdlib.h>

int main()
{
    int numElements;
    printf("Enter number of elements: ");
    scanf("%d", &numElements);

    int inputArray[numElements];
    for (int index = 0; index < numElements; index++)
    {
        printf("Enter element %d: ", index + 1);
        scanf("%d", &inputArray[index]);
    }

    FILE *dataFile;
    dataFile = fopen("data.txt", "w");
    for (int index = 0; index < numElements; index++)
    {
        putc(inputArray[index] + '0', dataFile);
        putc(' ', dataFile);
    }
    fclose(dataFile);

    int sortedArray[numElements];
    dataFile = fopen("data.txt", "r");
    for (int index = 0; index < numElements; index++)
    {
        fscanf(dataFile, "%d", &sortedArray[index]);
    }
    fclose(dataFile);

    for (int pass = 0; pass < numElements - 1; pass++)
    {
        for (int index = 0; index < numElements - 1 - pass; index++)
        {
            if (sortedArray[index] > sortedArray[index + 1])
            {
                int tempValue = sortedArray[index];
                sortedArray[index] = sortedArray[index + 1];
                sortedArray[index + 1] = tempValue;
            }
        }
    }

    printf("Sorted Array: ");
    for (int index = 0; index < numElements; index++)
    {
        printf("%d ", sortedArray[index]);
    }
    printf("\n");

    return 0;
}
