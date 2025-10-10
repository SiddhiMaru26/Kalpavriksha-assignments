#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <stdbool.h>

#define MAX_STUDENTS 100

struct Student
{
    int rollNumber;
    char name[50];
    int marks1;
    int marks2;
    int marks3;
};

bool isNumber(char *str)
{
    for (int i = 0; i < strlen(str); i++)
    {
        if (!isdigit((unsigned char)str[i]))
        {
            return false;
        }
    }
    return true;
}

bool isAlphaString(char *str)
{
    for (int i = 0; i < strlen(str); i++)
    {
        if (!isalpha((unsigned char)str[i]) && str[i] != ' ')
        {
            return false;
        }
    }
    return true;
}

bool marksValid(int m1, int m2, int m3)
{
    if (m1 < 0 || m1 > 100)
    {
        return false;
    }
    if (m2 < 0 || m2 > 100)
    {
        return false;
    }
    if (m3 < 0 || m3 > 100)
    {
        return false;
    }
    return true;
}

bool rollExists(struct Student students[], int count, int roll)
{
    for (int i = 0; i < count; i++)
    {
        if (students[i].rollNumber == roll)
        {
            return true;
        }
    }
    return false;
}

int calculateTotal(int mark1, int mark2, int mark3)
{
    return mark1 + mark2 + mark3;
}

float calculateAverage(struct Student student)
{
    return calculateTotal(student.marks1, student.marks2, student.marks3) / 3.0f;
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

    for (int i = 0; i < starCount; i++)
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
    char input[100];

    while (true)
    {
        printf("Enter the number of students: ");
        fgets(input, sizeof(input), stdin);
        input[strcspn(input, "\n")] = '\0';

        if (!isNumber(input))
        {
            printf("Invalid number of students. Must be between 1 and 100.\n");
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

    for (int i = 0; i < numberOfStudents; i++)
    {
        printf("\n--- Enter details for Student %d ---\n", i + 1);
        bool valid = false;

        while (!valid)
        {
            printf("Enter RollNumber Name Marks1 Marks2 Marks3: ");
            fgets(input, sizeof(input), stdin);
            input[strcspn(input, "\n")] = '\0';

            char rollStr[10];
            char name[50];
            int m1, m2, m3;

            int itemsRead = sscanf(input, "%9s %49s %d %d %d", rollStr, name, &m1, &m2, &m3);

            if (itemsRead != 5)
            {
                printf("Invalid input! Please follow the format: RollNumber Name Marks1 Marks2 Marks3\n");
                continue;
            }

            if (!isNumber(rollStr))
            {
                printf("Roll number must be a numeric value.\n");
                continue;
            }

            int rollNum = atoi(rollStr);

            if (rollNum <= 0)
            {
                printf("Invalid Roll Number! Must be greater than 0.\n");
                continue;
            }

            if (rollExists(students, i, rollNum))
            {
                printf("Duplicate roll number detected! Enter a unique roll number.\n");
                continue;
            }

            if (!isAlphaString(name))
            {
                printf("Invalid name! Only letters are allowed.\n");
                continue;
            }

            if (!marksValid(m1, m2, m3))
            {
                printf("Marks must be between 0 and 100.\n");
                continue;
            }

            students[i].rollNumber = rollNum;
            strcpy(students[i].name, name);
            students[i].marks1 = m1;
            students[i].marks2 = m2;
            students[i].marks3 = m3;

            valid = true;
        }
    }

    printf("\n--- Student Details ---\n");

    for (int i = 0; i < numberOfStudents; i++)
    {
        int totalMarks = calculateTotal(students[i].marks1, students[i].marks2, students[i].marks3);
        float averageMarks = calculateAverage(students[i]);
        char grade = assignGrade(averageMarks);

        printf("\nRoll: %d\n", students[i].rollNumber);
        printf("Name: %s\n", students[i].name);
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
