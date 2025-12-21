#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define MIN_SIZE 2
#define MAX_SZE 10
#define MIN_VALUE 0
#define MAX_VALUE 255

int validateMatrixSize()
{
    int matrixSize;
    printf("Enter matrix size (%d-%d): ", MIN_SIZE, MAX_SZE);
    scanf("%d", &matrixSize);

    if (matrixSize >= MIN_SIZE && matrixSize <= MAX_SZE)
    {
        return matrixSize;
    }
    else
    {
        printf("Invalid input. Please enter a number between %d and %d.\n", MIN_SIZE, MAX_SZE);
        return 0;
    }
}

void generateMatrix(int *matrixPointer, int dimension)
{
    for (int row = 0; row < dimension; row++)
    {
        for (int col = 0; col < dimension; col++)
        {
            *(matrixPointer + row * dimension + col) = MIN_VALUE + rand() % (MAX_VALUE - MIN_VALUE + 1);
        }
    }
}

void printMatrix(int *matrixPointer, int dimension, const char *title)
{
    printf("\n%s\n", title);
    for (int row = 0; row < dimension; row++)
    {
        for (int col = 0; col < dimension; col++)
        {
            printf("%3d ", *(matrixPointer + row * dimension + col));
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

void rotateMatrixClockwise(int *matrixPointer, int dimension)
{
    for (int row = 0; row < dimension - 1; row++)
    {
        for (int col = row + 1; col < dimension; col++)
        {
            swapValues(matrixPointer + row * dimension + col,
                       matrixPointer + col * dimension + row);
        }
    }

    for (int row = 0; row < dimension; row++)
    {
        for (int col = 0; col < dimension / 2; col++)
        {
            swapValues(matrixPointer + row * dimension + col,
                       matrixPointer + row * dimension + (dimension - 1 - col));
        }
    }
}

void smoothMatrix(int *matrixPointer, int dimension)
{
    int temp[dimension][dimension];

    for (int row = 0; row < dimension; row++)
    {
        for (int col = 0; col < dimension; col++)
        {
            int sum = 0;
            int count = 0;

            int startRow = (row > MIN_VALUE) ? row - 1 : row;
            int endRow = (row < dimension - 1) ? row + 1 : row;
            int startCol = (col > MIN_VALUE) ? col - 1 : col;
            int endCol = (col < dimension - 1) ? col + 1 : col;

            for (int r = startRow; r <= endRow; r++)
            {
                for (int c = startCol; c <= endCol; c++)
                {
                    sum += *(matrixPointer + r * dimension + c);
                    count++;
                }
            }

            temp[row][col] = sum / count;
        }
    }

    for (int row = 0; row < dimension; row++)
    {
        for (int col = 0; col < dimension; col++)
        {
            *(matrixPointer + row * dimension + col) = temp[row][col];
        }
    }
}

int main(void)
{
    srand((unsigned)time(NULL));

    int matrixDimension = validateMatrixSize();
    if (matrixDimension == 0)
        return 0;

    int matrix[matrixDimension][matrixDimension];
    int *matrixPointer = (int *)matrix;

    generateMatrix(matrixPointer, matrixDimension);
    printMatrix(matrixPointer, matrixDimension, "Original Randomly Generated Matrix:");

    rotateMatrixClockwise(matrixPointer, matrixDimension);
    printMatrix(matrixPointer, matrixDimension, "Rotate Matrix 90 degree Clockwise:");

    smoothMatrix(matrixPointer, matrixDimension);
    printMatrix(matrixPointer, matrixDimension, "Apply Smoothing Filter:");

    return 0;

}
