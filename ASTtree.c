/*
Author: Haitore
Date: September 2024
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

typedef enum
{
    Plus,
    Minus,
    Times,
    Divide,
    Modulo,
    Power
} OpKind; // Enum para las operaciones
typedef enum
{
    OpKindType,
    ConstKind,
    Exp,
    Term,
    SumaOp,
    MultOp,
    Fac,
    PotOp,
    Comp
} ExpKind; // Tipos de expresión

// Estructura para los nodos del árbol de sintaxis con anotaciones
typedef struct streenode
{
    ExpKind kind;             // Tipo de expresión: operación o constante
    OpKind op;                // Tipo de operación: Plus, Minus, Times, etc.
    struct streenode *lchild; // Nodo hijo izquierdo
    struct streenode *rchild; // Nodo hijo derecho
    int val;                  // Valor de la constante o el resultado de la operación
    char annotation[50];      // Anotaciones adicionales (ej. tipo de dato)
} STreeNode;

typedef STreeNode *SyntaxTree;

// Prototipos de funciones
SyntaxTree parseExpression(char **expr);
SyntaxTree parseSumaOp(char **expr);
SyntaxTree parseTerm(char **expr);
SyntaxTree parseMultOp(char **expr);
SyntaxTree parseFactor(char **expr);
SyntaxTree parsePower(char **expr);
SyntaxTree parseComp(char **expr);
void postEval(SyntaxTree t);
void printAnnotatedTree(SyntaxTree t, int level);

int main()
{
    char expr[100];

    // Pedir al usuario que ingrese una expresión
    printf("Ingresa una expresión aritmética: ");
    fgets(expr, 100, stdin);

    // Convertir el puntero a char para manejar la expresión
    char *p = expr;

    // Construir el árbol sintáctico a partir de la expresión
    SyntaxTree tree = parseExpression(&p);

    // Evaluar el árbol y agregar anotaciones
    postEval(tree);

    // Imprimir el árbol con las anotaciones
    printf("Árbol de sintaxis con anotaciones:\n");
    printAnnotatedTree(tree, 0);

    return 0;
}

// Función que evalúa el árbol y agrega anotaciones
void postEval(SyntaxTree t)
{
    if (t == NULL)
        return;

    if (t->kind == OpKindType) // Verificar si es un nodo de operación
    {
        postEval(t->lchild); // Evaluar el hijo izquierdo
        postEval(t->rchild); // Evaluar el hijo derecho
        switch (t->op)       // Aplicar la operación
        {
        case Plus:
            t->val = t->lchild->val + t->rchild->val;
            sprintf(t->annotation, "exp -> exp + term : %d + %d = %d", t->lchild->val, t->rchild->val, t->val);
            break;
        case Minus:
            t->val = t->lchild->val - t->rchild->val;
            sprintf(t->annotation, "exp -> exp - term : %d - %d = %d", t->lchild->val, t->rchild->val, t->val);
            break;
        case Times:
            t->val = t->lchild->val * t->rchild->val;
            sprintf(t->annotation, "term -> term * fac : %d * %d = %d", t->lchild->val, t->rchild->val, t->val);
            break;
        case Divide:
            t->val = t->lchild->val / t->rchild->val;
            sprintf(t->annotation, "term -> term / fac : %d / %d = %d", t->lchild->val, t->rchild->val, t->val);
            break;
        case Modulo:
            t->val = t->lchild->val % t->rchild->val;
            sprintf(t->annotation, "term -> term %% fac : %d %% %d = %d", t->lchild->val, t->rchild->val, t->val);
            break;
        case Power:
            t->val = 1;
            for (int i = 0; i < t->rchild->val; i++)
                t->val *= t->lchild->val;
            sprintf(t->annotation, "fac -> fac ^ comp : %d ^ %d = %d", t->lchild->val, t->rchild->val, t->val);
            break;
        }
    }
    else if (t->kind == ConstKind)
    {
        // Si es una constante, ya tiene un valor, agregar la anotación
        sprintf(t->annotation, "comp -> num : %d", t->val);
    }
}

// Función que imprime el árbol con las anotaciones
void printAnnotatedTree(SyntaxTree t, int level)
{
    if (t == NULL)
        return;

    // Imprimir el nodo actual con la anotación, indentando según el nivel
    for (int i = 0; i < level; i++)
        printf("       "); // Imprimir espacios para la indentación

    // Si es un nodo constante, imprimir su valor y anotación
    if (t->kind == ConstKind)
        printf("%d (%s)\n", t->val, t->annotation);
    else
        // Si es un nodo de operación, imprimir el operador y la anotación
        printf("%s (%s)\n", (t->op == Plus) ? "+" : (t->op == Minus) ? "-"
                                                : (t->op == Times)   ? "*"
                                                : (t->op == Divide)  ? "/"
                                                : (t->op == Modulo)  ? "%"
                                                                     : "^",
               t->annotation);

    // Imprimir el hijo izquierdo
    if (t->lchild)
        printAnnotatedTree(t->lchild, level + 1);

    // Imprimir el hijo derecho
    if (t->rchild)
        printAnnotatedTree(t->rchild, level + 1);
}

// Función que analiza la expresión y construye el árbol sintáctico
SyntaxTree parseExpression(char **expr)
{
    SyntaxTree node = parseTerm(expr); // Llama a parseTerm para manejar term

    // Mientras haya + o -
    while (**expr == '+' || **expr == '-')
    {
        SyntaxTree newNode = (SyntaxTree)malloc(sizeof(STreeNode));
        newNode->kind = OpKindType;
        newNode->op = (**expr == '+') ? Plus : Minus;

        (*expr)++; // Avanzar el puntero para pasar el operador

        newNode->lchild = node;
        newNode->rchild = parseTerm(expr); // Analiza el siguiente term
        node = newNode;
    }
    return node;
}

// Función para analizar términos (maneja * / %)
SyntaxTree parseTerm(char **expr)
{
    SyntaxTree node = parseFactor(expr);

    while (**expr == '*' || **expr == '/' || **expr == '%')
    {
        SyntaxTree newNode = (SyntaxTree)malloc(sizeof(STreeNode));
        newNode->kind = OpKindType;
        newNode->op = (**expr == '*') ? Times : (**expr == '/') ? Divide
                                                                : Modulo;

        (*expr)++; // Avanzar el puntero

        newNode->lchild = node;
        newNode->rchild = parseFactor(expr);
        node = newNode;
    }
    return node;
}

// Función para manejar factores y potencias
SyntaxTree parseFactor(char **expr)
{
    SyntaxTree node = parseComp(expr);

    if (**expr == '^')
    {
        SyntaxTree newNode = (SyntaxTree)malloc(sizeof(STreeNode));
        newNode->kind = OpKindType;
        newNode->op = Power;

        (*expr)++; // Avanzar el puntero

        newNode->lchild = node;
        newNode->rchild = parseComp(expr);
        node = newNode;
    }
    return node;
}

// Función para manejar comp -> (exp) | num
SyntaxTree parseComp(char **expr)
{
    SyntaxTree node = NULL;

    if (**expr == '(')
    {
        (*expr)++;                    // Saltar '('
        node = parseExpression(expr); // Analizar la subexpresión
        if (**expr == ')')
            (*expr)++; // Saltar ')'
    }
    else if (isdigit(**expr))
    {
        node = (SyntaxTree)malloc(sizeof(STreeNode));
        node->kind = ConstKind;
        node->val = 0;

        while (isdigit(**expr))
        {
            node->val = node->val * 10 + (**expr - '0');
            (*expr)++;
        }
    }

    return node;
}