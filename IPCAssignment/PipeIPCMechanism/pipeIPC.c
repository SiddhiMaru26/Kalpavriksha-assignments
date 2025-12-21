#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>

#define MAX_ELEMENTS 100

int getIntegerInput()
{
    int value;
    while (1)
    {
        if (scanf("%d", &value) != 1)
        {
            while (getchar() != '\n');
        }
        else
        {
            if (getchar() != '\n')
            {
                while (getchar() != '\n');
            }
            else
            {
                return value;
            }
        }
    }
}

int readArrayFromUser(int *array)
{
    int elementCount;
    elementCount = getIntegerInput();
    if (elementCount <= 0 || elementCount > MAX_ELEMENTS)
    {
        return -1;
    }
    for (int index = 0; index < elementCount; index++)
    {
        array[index] = getIntegerInput();
    }
    return elementCount;
}

void sortArray(int *array, int elementCount)
{
    for (int pass = 0; pass < elementCount - 1; pass++)
    {
        for (int index = 0; index < elementCount - 1 - pass; index++)
        {
            if (array[index] > array[index + 1])
            {
                int temp = array[index];
                array[index] = array[index + 1];
                array[index + 1] = temp;
            }
        }
    }
}

void parentProcessActions(int pipeDescriptors[2], int *array, int elementCount)
{
    close(pipeDescriptors[0]);
    write(pipeDescriptors[1], array, elementCount * sizeof(int));
    close(pipeDescriptors[1]);
}

void childProcessActions(int pipeDescriptors[2], int elementCount)
{
    close(pipeDescriptors[1]);
    int receivedArray[MAX_ELEMENTS];
    read(pipeDescriptors[0], receivedArray, elementCount * sizeof(int));
    close(pipeDescriptors[0]);
    sortArray(receivedArray, elementCount);
    for (int index = 0; index < elementCount; index++)
    {
        printf("%d ", receivedArray[index]);
    }
    printf("\n");
}

void executePipeBasedSorting()
{
    int pipeDescriptors[2];
    pipe(pipeDescriptors);

    int elementCount;
    elementCount = getIntegerInput();
    if (elementCount <= 0 || elementCount > MAX_ELEMENTS)
    {
        return;
    }

    int userArray[MAX_ELEMENTS];
    for (int index = 0; index < elementCount; index++)
    {
        userArray[index] = getIntegerInput();
    }

    pid_t processId = fork();

    if (processId < 0)
    {
        return;
    }
    else if (processId == 0)
    {
        parentProcessActions(pipeDescriptors, userArray, elementCount);
    }
    else
    {
        wait(NULL);
        childProcessActions(pipeDescriptors, elementCount);
    }
}

int main()
{
    executePipeBasedSorting();
    return 0;
}
