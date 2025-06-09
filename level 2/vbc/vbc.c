#include "vbc.h"

node *parse_nbr(char **s)
{
    if(!isdigit(**s))
    {
        unexpected(**s);
        return NULL;
    }
    node n = { .type = VAL, .val = **s - '0', .l = NULL, .r = NULL };
    (*s)++;
    return new_node(n);
}

node *parse_parenth(char **s)
{
    if (accept(s, '('))
    {
        node *expr = pars_expr(s);
        if(expect(s, ')'))
            return expr;
        else
        {
            destroy_tree(expr);
            return NULL;
        }
    }
    // 1. parse_nbr()
    return parse_nbr(s);
}

// Multiplication
node *parse_term(char **s)
{
    // 5. parentheses
    node *left = parse_parenth(s);
    while (accept(s, '*'))
    {
        node *right = parse_parenth(s);
        if (!right)
        {
            destroy_tree(left);
            return NULL;
        }
        node n = { .type = MULTI, .l = left, .r = right };
        left = new_node(n);
    }
    return (left);
}

node	*pars_expr(char **s)
{
    // 3. Multiplication.
    node *left = parse_term(s);
    // 2. Addition
    while (accept(s, '+'))
    {
        node *right = parse_term(s);
        if(!right)
        {
            destroy_tree(left);
            return NULL;
        }
        node n = { .type = ADD, .l = left, .r = right };
        left = new_node(n);
    }
    return (left);
}
