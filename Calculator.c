#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>

#define MAX_SIZE 300000  

void evaluateExpression() 
{
    char inputExpression[MAX_SIZE];
    printf("Enter the string you want to evaluate:\n");
    fgets(inputExpression, sizeof(inputExpression), stdin);

    char expression[MAX_SIZE];
    int exprIndex = 0; 

    // remove spaces
    for (int inputIndex = 0; inputIndex < strlen(inputExpression); inputIndex++)
    {
        if (!isspace((unsigned char)inputExpression[inputIndex])) 
        {
            expression[exprIndex++] = inputExpression[inputIndex];
        }
    }
    expression[exprIndex] = '\0';

    int length = strlen(expression);
    if (length == 0) 
    {
        printf("Error: Empty expression.\n");
        return;
    }

    if (expression[length - 1] == '\n') 
    {
        expression[length - 1] = '\0';
        length--;
    }
    if (length == 0) 
    {
        printf("Error: Empty expression.\n");
        return;
    }

    if (expression[0] == '*' || expression[0] == '/') 
    {
        printf("Error: Invalid operator placement\n");
        return;
    }

    for (int expIndex = 0; expression[expIndex] != '\0'; expIndex++) 
    {
        char currentChar = expression[expIndex];

        if (!isdigit((unsigned char)currentChar) &&
            currentChar != '+' && currentChar != '-' &&
            currentChar != '*' && currentChar != '/') 
        {
            printf("Error: Unexpected character\n");
            return;
        }

        if ((currentChar == '+' || currentChar == '-') &&
            (expIndex == 0 || expression[expIndex - 1] == '+' || expression[expIndex - 1] == '-' ||
             expression[expIndex - 1] == '*' || expression[expIndex - 1] == '/'))
        {
            if (expIndex + 1 < length && (expression[expIndex + 1] == '+' || expression[expIndex + 1] == '-')) 
            {
                printf("Error: Consecutive unary operators\n");
                return;
            }
        }

        if ((currentChar == '*' || currentChar == '/') &&
            (expIndex == length - 1 || expression[expIndex + 1] == '+' || expression[expIndex + 1] == '-' ||
             expression[expIndex + 1] == '*' || expression[expIndex + 1] == '/')) 
        {
            printf("Error: Invalid operator placement\n");
            return;
        }
    }

    int operandStack[MAX_SIZE];
    int top = -1;
    char operator = '+';
    int operand = 0;

    for (int expIndex = 0; expIndex < strlen(expression); expIndex++) 
    {
        if (isdigit((unsigned char)expression[expIndex])) 
        {
            operand = operand * 10 + (expression[expIndex] - '0');
        }

        if (!isdigit((unsigned char)expression[expIndex]) || expression[expIndex + 1] == '\0') 
        {
            if (operator == '+') 
            {
                if (top + 1 >= MAX_SIZE) 
                {
                    printf("Error: Stack Overflow\n");
                    return;
                }
                operandStack[++top] = operand;
            } 
            else if (operator == '-') 
            {
                if (top + 1 >= MAX_SIZE) 
                {
                    printf("Error: Stack Overflow\n");
                    return;
                }
                operandStack[++top] = -operand;
            } 
            else if (operator == '*') 
            {
                if (top < 0) 
                {
                    printf("Error: Stack Underflow\n");
                    return;
                }
                operandStack[top] = operandStack[top] * operand;
            } 
            else if (operator == '/') 
            {
                if (top < 0) 
                {
                    printf("Error: Stack Underflow\n");
                    return;
                }
                if (operand == 0) 
                {
                    printf("Error: Division by zero.\n");
                    return;
                }
                operandStack[top] = operandStack[top] / operand;
            }
            operator = expression[expIndex];
            operand = 0;
        }
    }

    int result = 0;
    for (int stackIndex = 0; stackIndex <= top; stackIndex++) 
    {
        result += operandStack[stackIndex];
    }

    printf("The result is: %d\n", result);
}

int main() 
{
    evaluateExpression();
    return 0;
}
