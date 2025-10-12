#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <stdbool.h>

#define MAX_STUDENTS 100
#define MARKS_COUNT 3

struct Student
{
    int rollNumber;
    char name[50];
    int marks[MARKS_COUNT];
};

bool isNumber(char *inputExpression)
{
    for (int index = 0; index < strlen(inputExpression); index++)
    {
        if (!isdigit((unsigned char)inputExpression[index]))
        {
            return false;
        }
    }
    return true;
}

bool isAlphaString(char *inputExpression)
{
    for (int index = 0; index < strlen(inputExpression); index++)
    {
        if (!isalpha((unsigned char)inputExpression[index]) && inputExpression[index] != ' ')
        {
            return false;
        }
    }
    return true;
}

bool areMarksValid(int marks[], int count)
{
    for (int index = 0; index < count; index++)
    {
        if (marks[index] < 0 || marks[index] > 100)
        {
            return false;
        }
    }
    return true;
}

bool doesRollExist(struct Student students[], int totalStudents, int rollNumber)
{
    for (int index = 0; index < totalStudents; index++)
    {
        if (students[index].rollNumber == rollNumber)
        {
            return true;
        }
    }
    return false;
}

int calculateTotal(int marks[], int subjectcount)
{
    int totalmarks = 0;

    for (int index = 0; index < subjectcount; index++)
    {
        totalmarks += marks[index];
    }

    return totalmarks;
}

float calculateAverage(int marks[], int subjectcount)
{
    return calculateTotal(marks, subjectcount) / (float)subjectcount;
}

char assignGrade(float average)
{
    if (average >= 85)
    {
        return 'A';
    }
    else if (average >= 70)
    {
        return 'B';
    }
    else if (average >= 50)
    {
        return 'C';
    }
    else if (average >= 35)
    {
        return 'D';
    }
    else
    {
        return 'F';
    }
}

void printPerformanceStars(char grade)
{
    int starCount = 0;

    switch (grade)
    {
        case 'A':
            starCount = 5;
            break;

        case 'B':
            starCount = 4;
            break;

        case 'C':
            starCount = 3;
            break;

        case 'D':
            starCount = 2;
            break;

        default:
            starCount = 0;
            break;
    }

    for (int index = 0; index < starCount; index++)
    {
        printf("*");
    }
}

void printRollNumbers(struct Student students[], int totalStudents, int index)
{
    if (index >= totalStudents)
    {
        return;
    }

    printf("%d", students[index].rollNumber);

    if (index < totalStudents - 1)
    {
        printf(" ");
    }

    printRollNumbers(students, totalStudents, index + 1);
}

int main()
{
    struct Student students[MAX_STUDENTS];
    int numberOfStudents;
    char input[200];

    while (true)
    {
        printf("Enter the number of students: ");
        fgets(input, sizeof(input), stdin);
        input[strcspn(input, "\n")] = '\0';

        if (!isNumber(input))
        {
            printf("Invalid number of students. Must be numeric.\n");
            continue;
        }

        numberOfStudents = atoi(input);

        if (numberOfStudents <= 0 || numberOfStudents > MAX_STUDENTS)
        {
            printf("Invalid number of students. Must be between 1 and 100.\n");
            continue;
        }

        break;
    }

    for (int index = 0; index < numberOfStudents; index++)
    {
      
      
        bool validInput = false;

        while (!validInput)
        {
            printf("Enter RollNumber Name Marks1 Marks2 Marks3: ");
            fgets(input, sizeof(input), stdin);
            input[strcspn(input, "\n")] = '\0';

            char rollNumberInput[10];
            char studentName[50];
            int marks[MARKS_COUNT];

            int valuesRead = sscanf(input, "%9s %49s %d %d %d",
                                    rollNumberInput, studentName,
                                    &marks[0], &marks[1], &marks[2]);

            if (valuesRead != 5)
            {
                printf("Invalid input! Format: RollNumber Name Marks1 Marks2 Marks3\n");
                continue;
            }

            if (!isNumber(rollNumberInput))
            {
                printf("Roll number must be numeric.\n");
                continue;
            }

            int rollNumber = atoi(rollNumberInput);

            if (rollNumber <= 0)
            {
                printf("Roll number must be greater than 0.\n");
                continue;
            }

            if (doesRollExist(students, index, rollNumber))
            {
                printf("Duplicate roll number detected! Enter unique roll number.\n");
                continue;
            }

            if (!isAlphaString(studentName))
            {
                printf("Invalid name! Only letters allowed.\n");
                continue;
            }

            if (!areMarksValid(marks, MARKS_COUNT))
            {
                printf("Marks must be between 0 and 100.\n");
                continue;
            }

            students[index].rollNumber = rollNumber;
            strcpy(students[index].name, studentName);

            for (int markIndex = 0; markIndex < MARKS_COUNT; markIndex++)
            {
                students[index].marks[markIndex] = marks[markIndex];
            }

            validInput = true;
        }
    }

    for (int index = 0; index < numberOfStudents; index++)
    {
        int totalMarks = calculateTotal(students[index].marks, MARKS_COUNT);
        float averageMarks = calculateAverage(students[index].marks, MARKS_COUNT);
        char grade = assignGrade(averageMarks);

        printf("\nRoll: %d\n", students[index].rollNumber);
        printf("Name: %s\n", students[index].name);
        printf("Total: %d\n", totalMarks);
        printf("Average: %.2f\n", averageMarks);
        printf("Grade: %c\n", grade);

        if (averageMarks >= 35)
        {
            printf("Performance: ");
            printPerformanceStars(grade);
            printf("\n");
        }
    }

    printf("\nList of Roll Numbers (via recursion): ");
    printRollNumbers(students, numberOfStudents, 0);
    printf("\n");

    return 0;
}
