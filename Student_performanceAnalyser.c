#include <stdio.h>
#include <string.h>

#define MAX_STUDENTS 100

struct Student {
    int rollNumber;
    char name[50];
    int marks1;
    int marks2;
    int marks3;
};

int calculateTotal(int mark1, int mark2, int mark3) {
    return mark1 + mark2 + mark3;
}

float calculateAverage(struct Student student) {
    return calculateTotal(student.marks1, student.marks2, student.marks3) / 3.0;
}


char assignGrade(float average) {
    if (average >= 85) return 'A';
    else if (average >= 70) return 'B';
    else if (average >= 50) return 'C';
    else if (average >= 35) return 'D';
    else return 'F';
}


void printPerformanceStars(char grade) {
    int starCount = 0;
    switch (grade) {
        case 'A': starCount = 5; break;
        case 'B': starCount = 4; break;
        case 'C': starCount = 3; break;
        case 'D': starCount = 2; break;
        default: starCount = 0; break;
    }
    for (int i = 0; i < starCount; i++) {
        printf("*");
    }
}


void printRollNumbers(struct Student students[], int totalStudents, int index) {
    if (index >= totalStudents) return;
    printf("%d", students[index].rollNumber);
    if (index < totalStudents - 1) printf(" ");
    printRollNumbers(students, totalStudents, index + 1);
}

int main() {
    int numberOfStudents;
    scanf("%d", &numberOfStudents);
    getchar(); 

    struct Student students[MAX_STUDENTS];

    for (int i = 0; i < numberOfStudents; i++) {
        char inputLine[100];
        int itemsRead = 0;

        do 
        {
            fgets(inputLine, sizeof(inputLine), stdin);
            itemsRead = sscanf(inputLine, "%d %49s %d %d %d",
                               &students[i].rollNumber,
                               students[i].name,
                               &students[i].marks1,
                               &students[i].marks2,
                               &students[i].marks3);
            if (itemsRead != 5) 
            {
                printf("Invalid input! Please enter: RollNumber Name Marks1 Marks2 Marks3\n");
            }
        } while (itemsRead != 5);
    }


    for (int i = 0; i < numberOfStudents; i++) {
        int totalMarks = calculateTotal(students[i].marks1, students[i].marks2, students[i].marks3);
        float averageMarks = calculateAverage(students[i]);
        char grade = assignGrade(averageMarks);

        printf("Roll: %d\n", students[i].rollNumber);
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
        printf("\n");
    }

    
    printf("List of Roll Numbers (via recursion): ");
    printRollNumbers(students, numberOfStudents, 0);
    printf("\n");

    return 0;
}
