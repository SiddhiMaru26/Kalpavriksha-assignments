#include <stdio.h>
#include <stdlib.h>
#include <time.h>

int validateMatrixSize()
{
    int matrixSize;
    printf("Enter matrix size (2-10): ");
    scanf("%d", &matrixSize);

    if (matrixSize >= 2 && matrixSize <= 10)
    {
        return matrixSize;
    }
    else
    {
        printf("Invalid input. Please enter a number between 2 and 10.\n");
        return 0;
    }
}

void generateAndPrintMatrix(int *matrixPointer, int dimension)
{
    printf("\nOriginal Randomly Generated Matrix:\n");
    for (int row = 0; row < dimension; row++)
    {
        for (int column = 0; column < dimension; column++)
        {
            *(matrixPointer + row * dimension + column) = rand() % 256;
            printf("%d ", *(matrixPointer + row * dimension + column));
        }
        printf("\n");
    }
}

void swapValues(int *firstValue, int *secondValue)
{
    int temporary = *firstValue;
    *firstValue = *secondValue;
    *secondValue = temporary;
}

void rotateMatrix90Clockwise(int *matrixPointer, int dimension)
{
    for (int row = 0; row < dimension - 1; row++)
    {
        for (int column = row + 1; column < dimension; column++)
        {
            swapValues(matrixPointer + row * dimension + column,
                       matrixPointer + column * dimension + row);
        }
    }

    for (int row = 0; row < dimension; row++)
    {
        for (int column = 0; column < dimension / 2; column++)
        {
            swapValues(matrixPointer + row * dimension + column,
                       matrixPointer + row * dimension + (dimension - 1 - column));
        }
    }

    printf("\nMatrix after 90° Clockwise Rotation:\n");
    for (int row = 0; row < dimension; row++)
    {
        for (int column = 0; column < dimension; column++)
        {
            printf("%d ", *(matrixPointer + row * dimension + column));
        }
        printf("\n");
    }
}

void smoothMatrix(int *matrixPointer, int dimension)
{
    int previousRow[10];
    int currentRow[10];
    int previousRowComputed = 0;

    for (int row = 0; row < dimension; row++)
    {
        for (int column = 0; column < dimension; column++)
        {
            int sum = 0;
            int count = 0;

            int startRow = (row > 0) ? row - 1 : row;
            int endRow = (row < dimension - 1) ? row + 1 : row;
            int startColumn = (column > 0) ? column - 1 : column;
            int endColumn = (column < dimension - 1) ? column + 1 : column;

            int *currentPointer = matrixPointer + startRow * dimension + startColumn;

            for (int neighborRow = startRow; neighborRow <= endRow; neighborRow++)
            {
                for (int neighborColumn = 0; neighborColumn <= endColumn - startColumn; neighborColumn++)
                {
                    sum += *(currentPointer + neighborColumn);
                    count++;
                }
                currentPointer += dimension;
            }

            currentRow[column] = sum / count;
        }

        if (previousRowComputed)
        {
            int *previousRowPointer = matrixPointer + (row - 1) * dimension;
            for (int column = 0; column < dimension; column++)
            {
                *(previousRowPointer + column) = previousRow[column];
            }
        }

        for (int column = 0; column < dimension; column++)
        {
            previousRow[column] = currentRow[column];
        }

        previousRowComputed = 1;
    }

    if (previousRowComputed)
    {
        int *previousRowPointer = matrixPointer + (dimension - 1) * dimension;
        for (int column = 0; column < dimension; column++)
        {
            *(previousRowPointer + column) = previousRow[column];
        }
    }

    printf("\nMatrix after Applying 3x3 Smoothing Filter:\n");
    for (int row = 0; row < dimension; row++)
    {
        for (int column = 0; column < dimension; column++)
        {
            printf("%d ", *(matrixPointer + row * dimension + column));
        }
        printf("\n");
    }
}

int main(void)
{
    srand(time(0));

    int matrixDimension = validateMatrixSize();

    if (matrixDimension == 0)
    {
        return 0;
    }

    int matrix[matrixDimension][matrixDimension];
    int *matrixPointer = (int *)matrix;

    generateAndPrintMatrix(matrixPointer, matrixDimension);
    rotateMatrix90Clockwise(matrixPointer, matrixDimension);
    smoothMatrix(matrixPointer, matrixDimension);

    return 0;
}
