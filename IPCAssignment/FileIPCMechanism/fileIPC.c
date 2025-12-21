#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>

#define MAX_ELEMENTS 100
#define DATA_FILE "data.txt"

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

void printArray(int *array, int elementCount)
{
    for (int index = 0; index < elementCount; index++)
    {
        printf("%d ", array[index]);
    }
    printf("\n");
}

void saveArrayToFile(int *array, int elementCount)
{
    FILE *file = fopen(DATA_FILE, "w");
    if (!file)
    {
        return;
    }
    fprintf(file, "%d\n", elementCount);
    for (int index = 0; index < elementCount; index++)
    {
        fprintf(file, "%d ", array[index]);
    }
    fclose(file);
}

int loadArrayFromFile(int *array)
{
    FILE *file = fopen(DATA_FILE, "r");
    if (!file)
    {
        return 0;
    }
    int elementCount;
    fscanf(file, "%d", &elementCount);
    for (int index = 0; index < elementCount; index++)
    {
        fscanf(file, "%d", &array[index]);
    }
    fclose(file);
    return elementCount;
}

void bubbleSortArray(int *array, int elementCount)
{
    for (int index = 0; index < elementCount - 1; index++)
    {
        for (int innerIndex = 0; innerIndex < elementCount - index - 1; innerIndex++)
        {
            if (array[innerIndex] > array[innerIndex + 1])
            {
                int temp = array[innerIndex];
                array[innerIndex] = array[innerIndex + 1];
                array[innerIndex + 1] = temp;
            }
        }
    }
}

void parentProcessActions(int *array, int elementCount)
{
    saveArrayToFile(array, elementCount);
}

void childProcessActions()
{
    int childArray[MAX_ELEMENTS];
    int childCount = loadArrayFromFile(childArray);
    bubbleSortArray(childArray, childCount);
    saveArrayToFile(childArray, childCount);
}

void executeFileBasedIPC()
{
    int userArray[MAX_ELEMENTS];
    int elementCount;

    elementCount = readArrayFromUser(userArray);
    if (elementCount == -1)
    {
        return;
    }

    parentProcessActions(userArray, elementCount);

    pid_t processId = fork();

    if (processId < 0)
    {
        return;
    }
    else if (processId == 0)
    {
        childProcessActions();
        exit(0);
    }
    else
    {
        wait(NULL);
        int sortedArray[MAX_ELEMENTS];
        int sortedCount = loadArrayFromFile(sortedArray);
        printArray(sortedArray, sortedCount);
    }
}

int main()
{
    executeFileBasedIPC();
    return 0;
}
