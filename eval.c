#include "eval.h"
#include <assert.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>

static double parseexpr(char** expression);
static double parseterm(char** expression);
static double parsefact(char** expression);
static void   skipspaces(char** expression);

void skipspaces(char** expression) {
    while (**expression && isspace(**expression))
        (*expression)++;
}

double parsefact(char** expression) {
    skipspaces(expression);
    double result;

    if (**expression == '(') {
        (*expression)++; 
        result = parseexpr(expression);
        if(**expression != ')') {
          printf("Expected a ')'.\n");
          return 0.0f;
        }
        (*expression)++;
    } else {
        char* endptr;
        result = strtod(*expression, &endptr);
        if(*expression == endptr) {
          printf("Expected a number.\n");
          return 0.0f;
        }
        *expression = endptr;
    }

    return result;
}

double parseterm(char** expression) {
    double result = parsefact(expression);
    skipspaces(expression);

    while (**expression == '*' || **expression == '/') {
        char op = **expression;
        (*expression)++; 
        double next = parsefact(expression);
        if (op == '*')
            result *= next;
        else if (next != 0)
            result /= next;
        else {
            printf("Division by zero.\n");
            return 0.0f;
        }
        skipspaces(expression);
    }

    return result;
}

double parseexpr(char** expression) {
    double result = parseterm(expression);
    skipspaces(expression);

    while (**expression == '+' || **expression == '-') {
        char op = **expression;
        (*expression)++; 
        double nextTerm = parseterm(expression);
        if (op == '+')
            result += nextTerm;
        else
            result -= nextTerm;
        skipspaces(expression);
    }
    return result;
}

double evalexpr(char* expression) {
    return parseexpr(&expression);
}
