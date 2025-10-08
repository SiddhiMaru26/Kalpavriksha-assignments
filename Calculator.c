#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>

#define MAX_SIZE 300000  // Avoid magic number

void evaluateExpression() {
    char inputExpression[MAX_SIZE];
    printf("Enter the string you want to evaluate:\n");
    fgets(inputExpression, sizeof(inputExpression), stdin);

    // Remove spaces from input
    char expression[MAX_SIZE];
    int j = 0;
    for (int i = 0; i < strlen(inputExpression); i++) {
        if (!isspace((unsigned char)inputExpression[i])) {
            expression[j++] = inputExpression[i];
        }
    }
    expression[j] = '\0';

    // Validate expression
    int length = strlen(expression);
    if (length == 0) {
        printf("Error: Empty expression.\n");
        return;
    }

    // Remove newline character if present
    if (expression[length - 1] == '\n') {
        expression[length - 1] = '\0';
        length--;
    }

    // Expression should not start with * or /
    if (expression[0] == '*' || expression[0] == '/') {
        printf("Error: Expression cannot start with '*' or '/'.\n");
        return;
    }

    for (int i = 0; expression[i] != '\0'; i++) {
        char currentChar = expression[i];
        if (!isdigit((unsigned char)currentChar) &&
            currentChar != '+' && currentChar != '-' &&
            currentChar != '*' && currentChar != '/') {
            printf("Error: Invalid Expression\n", currentChar);
            return;
        }
    }

    int operandStack[MAX_SIZE];
    int top = -1;
    char operator = '+';
    int operand = 0;

    // Evaluate the expression
    for (int i = 0; i < strlen(expression); i++) {
        if (isdigit((unsigned char)expression[i])) {
            operand = operand * 10 + (expression[i] - '0');
        }

        if (!isdigit((unsigned char)expression[i]) || expression[i + 1] == '\0') {
            if (operator == '+') {
                if (top + 1 >= MAX_SIZE) {
                    printf("Error: Stack Overflow\n");
                    return;
                }
                operandStack[++top] = operand;
            } 
            else if (operator == '-') {
                if (top + 1 >= MAX_SIZE) {
                    printf("Error: Stack Overflow\n");
                    return;
                }
                operandStack[++top] = -operand;
            } 
            else if (operator == '*') {
                if (top < 0) {
                    printf("Error: Stack Underflow\n");
                    return;
                }
                operandStack[top] = operandStack[top] * operand;
            } 
            else if (operator == '/') {
                if (top < 0) {
                    printf("Error: Stack Underflow\n");
                    return;
                }
                if (operand == 0) {
                    printf("Error: Division by zero.\n");
                    return;
                }
                operandStack[top] = operandStack[top] / operand;
            }
            operator = expression[i];
            operand = 0;
        }
    }

    int result = 0;
    for (int i = 0; i <= top; i++) {
        result += operandStack[i];
    }

    printf("The result is: %d\n", result);
}

int main() {
    evaluateExpression();
    return 0;
}
