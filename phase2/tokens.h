/*
 * File:	tokens.h
 *
 * Description:	This file contains the token definitions for use by the
 *		lexical analyzer and parser for Simple C.  Single character
 *		tokens use their ASCII values, so we can refer to them
 *		either as character literals or as symbolic names.
 */

# ifndef TOKENS_H
# define TOKENS_H

enum {
    ASSIGN = '=', LTN = '<', GTN = '>', PLUS = '+', MINUS = '-',
    STAR = '*', DIV = '/', REM = '%', ADDR = '&', NOT = '!', DOT = '.',
    LPAREN = '(', RPAREN = ')', LBRACK = '[', RBRACK = ']',
    LBRACE = '{', RBRACE = '}', SEMI = ';', COLON = ':', COMMA = ',',

    AUTO = 256, BREAK, CASE, CHAR, CONST, CONTINUE, DEFAULT, DO, DOUBLE,
    ELSE, ENUM, EXTERN, FLOAT, FOR, GOTO, IF, INT, LONG, REGISTER,
    RETURN, SHORT, SIGNED, SIZEOF, STATIC, STRUCT, SWITCH, TYPEDEF,
    UNION, UNSIGNED, VOID, VOLATILE, WHILE,

    OR, AND, EQL, NEQ, LEQ, GEQ, INC, DEC, ARROW,
    ID, NUM, STRING, CHARACTER, ILLEGAL, DONE
};

void match(int t);
void orExpression();
void andf();
void eql();
void compr();
void expr();
void term();
void factor();
void point();
void expression();
void assignment();
void statements();
void statement();
void transunit();
void type_def();
int specifier();
void pointers();
void parameters();
void parameter();
void declaration();
void declarations();
void declarator();
void fog();
void remainingDeclarators();
void globalDeclaration();
void globalDeclarator();


# endif /* TOKENS_H */
