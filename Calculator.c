#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>

int main()
{
  char str[300000];
  printf("Enter the string you want to evaluate:\n");
  fgets(str, sizeof(str), stdin);
  fflush(stdin);

  // removing spaces from the string
  char temp[300000];
  int j = 0;
  for (int i = 0; i < strlen(str); i++)
  {
    if (str[i] != ' ')
    {
      temp[j] = str[i];
      j++;
    }
  }
  temp[j] = '\0';

  // Validating the string
  int len = strlen(temp);
  temp[len - 1] = '\0';
  for (int i = 0; temp[i] != '\0'; i++)
  {
    char ch = temp[i];
    if (!isdigit(ch))
    {
      if (ch != '+' && ch != '-' && ch != '*' && ch != '/')
      {
        printf("ERROR!!! INVALID INPUT");
        exit(1);
      }
    }
  }

  int stack[300000];
  int top = -1;
  int sign = 1;
  char op = '+';
  int num = 0;

  // evaluating the result
  for (int i = 0; i < strlen(temp); i++)
  {
    if (isdigit(temp[i]))
    {
      num = num * 10 + (temp[i] - '0');
    }
    if (!isdigit(temp[i]) || temp[i + 1] == '\0')
    {
      if (op == '+')
      {
        top++;
        stack[top] = num;
      }
      else if (op == '-')
      {
        top++;
        stack[top] = num * -1;
      }
      else if (op == '*')
      {
        int temp = stack[top];
        temp = temp * num;
        stack[top] = temp;
      }
      else if (op == '/')
      {
        int temp = stack[top];
        temp = temp / num;
        stack[top] = temp;
      }
      op = temp[i];
      num = 0;
    }
  }

  int result = 0;
  for (int i = 0; i <= top; i++)
  {
    result += stack[i];
  }

  printf("The result is:%d", result);
  return 0;
}