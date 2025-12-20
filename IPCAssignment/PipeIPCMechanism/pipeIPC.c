#include <stdio.h>
#include <unistd.h>

int main()
{
    int pipeDescriptors[2];
    pipe(pipeDescriptors);

    int numElements;
    printf("Enter number of elements: ");
    scanf("%d", &numElements);

    int inputArray[numElements];
    for (int index = 0; index < numElements; index++)
    {
        printf("Enter element %d: ", index + 1);
        scanf("%d", &inputArray[index]);
    }

    int processId = fork();

    if (processId == 0)
    {
        close(pipeDescriptors[0]);
        write(pipeDescriptors[1], inputArray, sizeof(inputArray));
        close(pipeDescriptors[1]);
    }
    else
    {
        close(pipeDescriptors[1]);
        int sortedArray[numElements];
        read(pipeDescriptors[0], sortedArray, sizeof(sortedArray));
        close(pipeDescriptors[0]);

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
    }

    return 0;
}
