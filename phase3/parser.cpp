/*
 * File:	parser.cpp
 *
 * Description:	This file contains the public and private function and
 *		variable definitions for the recursive-descent parser for
 *		Simple C.
 */

# include <cstdlib>
# include <iostream>
# include "tokens.h"
# include "lexer.h"
# include "checker.h"

using namespace std;

static int lookahead;
static string lexbuf;

static void expression();
static void statement();
Checker check;
//Checker::openScope();
//Checker::closeScope();
/*
void openScope() {
	//string type = specifier();
	cout << "open scope" << endl;
}

void closeScope() {
	cout << "close scope\n";
}
*/
/*
 * Function:	error
 *
 * Description:	Report a syntax error to standard error.
 */

static void error()
{
    if (lookahead == DONE)
	report("syntax error at end of file");
    else
	report("syntax error at '%s'", lexbuf);

    exit(EXIT_FAILURE);
}


/* Function:	match
 *
 * Description:	Match the next token against the specified token.  A
 *		failure indicates a syntax error and will terminate the
 *		program since our parser does not do error recovery.
 */

static void match(int t)
{
    if (lookahead != t)
	error();

    lookahead = lexan(lexbuf);
}


/*
 * Function:	isSpecifier
 *
 * Description:	Return whether the given token is a type specifier.
 */

static bool isSpecifier(int token)
{
    return token == INT || token == CHAR || token == STRUCT;
}


/*
 * Function:	specifier
 *
 * Description:	Parse a type specifier.  Simple C has int, char, and
 *		structure types.
 *
 *		specifier:
 *		  int
 *		  char
 *		  struct identifier
 */

string identifier() {
	string buf = lexbuf;
	match(ID);
	return buf;
}

string number() {
	string buf = lexbuf;
	match(NUM);
	return buf;
}

string specifier() {
	if (lookahead == INT) {
		match(INT);
		return "int";
	}
	if (lookahead == CHAR) {
		match(CHAR);
		return "char";
	}
	match(STRUCT);
	return identifier();
}


/*
 * Function:	pointers
 *
 * Description:	Parse pointer declarators (i.e., zero or more asterisks).
 *
 *		pointers:
 *		  empty
 *		  * pointers
 */

unsigned pointers() {
	unsigned count = 0;
	while (lookahead == '*') {
		match('*');
		count++;
	}
	return count;
}


/*
 * Function:	declarator
 *
 * Description:	Parse a declarator, which in Simple C is either a scalar
 *		variable or an array, both with optional pointer
 *		declarators, or a callback (i.e., a simple function
 *		pointer).
 *
 *		declarator:
 *		  pointers identifier
 *		  pointers identifier [ num ]
 *		  pointers ( * identifier ) ( )
 */

void declarator(const string &typespec) {
    unsigned indirection = pointers();
	if (lookahead == '(') {
		match('(');
		match('*');
		string name = identifier();
		check.declareVariable(name, Type(CALLBACK, typespec, indirection));
		match(')');
		match('(');
		match(')');
		cout << "(" << typespec << name << ", CALLBACK)" << endl;
	}
	else {
		string name = identifier();
		if (lookahead == '[') {
			match('[');
			string num = number();
			match(']');
			check.declareVariable(name, Type(ARRAY, typespec, indirection));
			cout << "(" << typespec << name << ", ARRAY)" << endl;
		} else {
			check.declareVariable(name, Type(SCALAR, typespec, indirection));
			cout << "(" << typespec << name << ", SCALAR)" << endl;
		}
	}
}


/*
 * Function:	declaration
 *
 * Description:	Parse a local variable declaration.  Global declarations
 *		are handled separately since we need to detect a function
 *		as a special case.
 *
 *		declaration:
 *		  specifier declarator-list ';'
 *
 *		declarator-list:
 *		  declarator
 *		  declarator , declarator-list
 */

void declaration() {
	string typespec = specifier();
	declarator(typespec);
	while (lookahead == ',') {
		match(',');
		declarator(typespec);
	}
	match(';');
}


/*
 * Function:	declarations
 *
 * Description:	Parse a possibly empty sequence of declarations.
 *
 *		declarations:
 *		  empty
 *		  declaration declarations
 */

static void declarations()
{
    while (isSpecifier(lookahead))
	declaration();
}


/*
 * Function:	primaryExpression
 *
 * Description:	Parse a primary expression.
 *
 *		primary-expression:
 *		  ( expression )
 *		  identifier
 *		  character
 *		  string
 *		  num
 */

static void primaryExpression(bool lparen)
{
    if (lparen) {
	expression();
	match(')');

    } else if (lookahead == CHARACTER)
	match(CHARACTER);

    else if (lookahead == STRING)
	match(STRING);

    else if (lookahead == NUM)
	match(NUM);

    else if (lookahead == ID) {
string thing = identifier();
cout << thing << " being checked\n";
	check.checkID(thing);
	}

    else
	error();
}


/*
 * Function:	postfixExpression
 *
 * Description:	Parse a postfix expression.
 *
 *		postfix-expression:
 *		  primary-expression
 *		  postfix-expression [ expression ]
 *		  postfix-expression ( expression-list )
 *		  postfix-expression ( )
 *		  postfix-expression . identifier
 *		  postfix-expression -> identifier
 *
 *		expression-list:
 *		  expression
 *		  expression , expression-list
 */

static void postfixExpression(bool lparen)
{
    primaryExpression(lparen);

    while (1) {
	if (lookahead == '[') {
	    match('[');
	    expression();
	    match(']');
	    ////cout << "index" << endl;

	} else if (lookahead == '(') {
	    match('(');

	    if (lookahead != ')') {
		expression();

		while (lookahead == ',') {
		    match(',');
		    expression();
		}
	    }

	    match(')');
	    //cout << "call" << endl;

	} else if (lookahead == '.') {
	    match('.');
	    identifier();
	    //cout << "dot" << endl;

	} else if (lookahead == ARROW) {
	    match(ARROW);
	    identifier();
	    //cout << "arrow" << endl;

	} else
	    break;
    }
}


/*
 * Function:	prefixExpression
 *
 * Description:	Parse a prefix expression.
 *
 *		prefix-expression:
 *		  postfix-expression
 *		  ! prefix-expression
 *		  - prefix-expression
 *		  * prefix-expression
 *		  & prefix-expression
 *		  sizeof prefix-expression
 *		  sizeof ( specifier pointers )
 *		  ( specifier pointers ) prefix-expression
 *
 *		This grammar is still ambiguous since "sizeof(type) * n"
 *		could be interpreted as a multiplicative expression or as a
 *		cast of a dereference.  The correct interpretation is the
 *		former, as the latter makes little sense semantically.  We
 *		resolve the ambiguity here by always consuming the "(type)"
 *		as part of the sizeof expression.
 */

static void prefixExpression()
{
    if (lookahead == '!') {
	match('!');
	prefixExpression();
	//cout << "not" << endl;

    } else if (lookahead == '-') {
	match('-');
	prefixExpression();
	//cout << "neg" << endl;

    } else if (lookahead == '*') {
	match('*');
	prefixExpression();
	//cout << "deref" << endl;

    } else if (lookahead == '&') {
	match('&');
	prefixExpression();
	//cout << "addr" << endl;

    } else if (lookahead == SIZEOF) {
	match(SIZEOF);

	if (lookahead == '(') {
	    match('(');

	    if (isSpecifier(lookahead)) {
		specifier();
		pointers();
		match(')');
	    } else
		postfixExpression(true);

	} else
	    prefixExpression();

	//cout << "sizeof" << endl;

    } else if (lookahead == '(') {
	match('(');

	if (isSpecifier(lookahead)) {
	    specifier();
	    pointers();
	    match(')');
	    prefixExpression();
	    //cout << "cast" << endl;
	} else
	    postfixExpression(true);

    } else
	postfixExpression(false);
}


/*
 * Function:	multiplicativeExpression
 *
 * Description:	Parse a multiplicative expression.
 *
 *		multiplicative-expression:
 *		  prefix-expression
 *		  multiplicative-expression * prefix-expression
 *		  multiplicative-expression / prefix-expression
 *		  multiplicative-expression % prefix-expression
 */

static void multiplicativeExpression()
{
    prefixExpression();

    while (1) {
	if (lookahead == '*') {
	    match('*');
	    prefixExpression();
	    //cout << "mul" << endl;

	} else if (lookahead == '/') {
	    match('/');
	    prefixExpression();
	    //cout << "div" << endl;

	} else if (lookahead == '%') {
	    match('%');
	    prefixExpression();
	    //cout << "rem" << endl;

	} else
	    break;
    }
}


/*
 * Function:	additiveExpression
 *
 * Description:	Parse an additive expression.
 *
 *		additive-expression:
 *		  multiplicative-expression
 *		  additive-expression + multiplicative-expression
 *		  additive-expression - multiplicative-expression
 */

static void additiveExpression()
{
    multiplicativeExpression();

    while (1) {
	if (lookahead == '+') {
	    match('+');
	    multiplicativeExpression();
	    //cout << "add" << endl;

	} else if (lookahead == '-') {
	    match('-');
	    multiplicativeExpression();
	    //cout << "sub" << endl;

	} else
	    break;
    }
}


/*
 * Function:	relationalExpression
 *
 * Description:	Parse a relational expression.  Note that Simple C does not
 *		have shift operators, so we go immediately to additive
 *		expressions.
 *
 *		relational-expression:
 *		  additive-expression
 *		  relational-expression < additive-expression
 *		  relational-expression > additive-expression
 *		  relational-expression <= additive-expression
 *		  relational-expression >= additive-expression
 */

static void relationalExpression()
{
    additiveExpression();

    while (1) {
	if (lookahead == '<') {
	    match('<');
	    additiveExpression();
	    //cout << "ltn" << endl;

	} else if (lookahead == '>') {
	    match('>');
	    additiveExpression();
	    //cout << "gtn" << endl;

	} else if (lookahead == LEQ) {
	    match(LEQ);
	    additiveExpression();
	    //cout << "leq" << endl;

	} else if (lookahead == GEQ) {
	    match(GEQ);
	    additiveExpression();
	    //cout << "geq" << endl;

	} else
	    break;
    }
}


/*
 * Function:	equalityExpression
 *
 * Description:	Parse an equality expression.
 *
 *		equality-expression:
 *		  relational-expression
 *		  equality-expression == relational-expression
 *		  equality-expression != relational-expression
 */

static void equalityExpression()
{
    relationalExpression();

    while (1) {
	if (lookahead == EQL) {
	    match(EQL);
	    relationalExpression();
	    //cout << "eql" << endl;

	} else if (lookahead == NEQ) {
	    match(NEQ);
	    relationalExpression();
	    //cout << "neq" << endl;

	} else
	    break;
    }
}


/*
 * Function:	logicalAndExpression
 *
 * Description:	Parse a logical-and expression.  Note that Simple C does
 *		not have bitwise-and expressions.
 *
 *		logical-and-expression:
 *		  equality-expression
 *		  logical-and-expression && equality-expression
 */

static void logicalAndExpression()
{
    equalityExpression();

    while (lookahead == AND) {
	match(AND);
	equalityExpression();
	//cout << "and" << endl;
    }
}


/*
 * Function:	expression
 *
 * Description:	Parse an expression, or more specifically, a logical-or
 *		expression, since Simple C does not allow comma or
 *		assignment as an expression operator.
 *
 *		expression:
 *		  logical-and-expression
 *		  expression || logical-and-expression
 */

static void expression()
{
    logicalAndExpression();

    while (lookahead == OR) {
	match(OR);
	logicalAndExpression();
	//cout << "or" << endl;
    }
}


/*
 * Function:	statements
 *
 * Description:	Parse a possibly empty sequence of statements.  Rather than
 *		checking if the next token starts a statement, we check if
 *		the next token ends the sequence, since a sequence of
 *		statements is always terminated by a closing brace.
 *
 *		statements:
 *		  empty
 *		  statement statements
 */

static void statements()
{
    while (lookahead != '}')
	statement();
}


/*
 * Function:	assignment
 *
 * Description:	Parse an assignment statement.
 *
 *		assignment:
 *		  expression = expression
 *		  expression
 */

static void assignment()
{
    expression();

    if (lookahead == '=') {
	match('=');
	expression();
    }
}


/*
 * Function:	statement
 *
 * Description:	Parse a statement.  Note that Simple C has so few
 *		statements that we handle them all in this one function.
 *
 *		statement:
 *		  { declarations statements }
 *		  return expression ;
 *		  while ( expression ) statement
 *		  for ( assignment ; expression ; assignment ) statement
 *		  if ( expression ) statement
 *		  if ( expression ) statement else statement
 *		  assignment ;
 */

static void statement()
{
    if (lookahead == '{') {
	match('{');
	check.openScope();

	declarations();
	statements();
	check.closeScope();
	match('}');

    } else if (lookahead == RETURN) {
	match(RETURN);
	expression();
	match(';');

    } else if (lookahead == WHILE) {
	match(WHILE);
	match('(');
	expression();
	match(')');
	statement();

    } else if (lookahead == FOR) {
	match(FOR);
	match('(');
	assignment();
	match(';');
	expression();
	match(';');
	assignment();
	match(')');
	statement();

    } else if (lookahead == IF) {
	match(IF);
	match('(');
	expression();
	match(')');
	statement();

	if (lookahead == ELSE) {
	    match(ELSE);
	    statement();
	}

    } else {
	assignment();
	match(';');
    }
}


/*
 * Function:	parameter
 *
 * Description:	Parse a parameter, which in Simple C is always either a
 *		simple variable with optional pointer declarators, or a
 *		callback (i.e., a simple function pointer)
 *
 *		parameter:
 *		  specifier pointers identifier
 *		  specifier pointers ( * identifier ) ( )
 */

Type parameter()
{
    string typespec = specifier();
	unsigned indirection = pointers();
    if (lookahead == '(') {
	match('(');
	match('*');
	string name = identifier();
	match(')');
	match('(');
	match(')');
	Type t = Type(CALLBACK, typespec, indirection);
	check.checkIfStructure(name, t);

	check.declareVariable(name, t);
//check.checkIfStructure(name, t);
	cout << "(" << typespec << name << ", CALLBACK)" << endl;
	return t;
    } else {
	//identifier();
	Type t = Type(SCALAR, typespec, indirection);
	string name = identifier();
	check.checkIfStructure(name, Type(CALLBACK, typespec, indirection));
	check.declareVariable(name, t);	
	cout << "(" << typespec << ", SCALAR)\n";
	return t;	
}
	
	
	
}


/*
 * Function:	parameters
 *
 * Description:	Parse the parameters of a function, but not the opening or
 *		closing parentheses.
 *
 *		parameters:
 *		  void
 *		  parameter-list
 *
 *		parameter-list:
 *		  parameter
 *		  parameter , parameter-list
 */

void parameters(Type &t)
{
    if (lookahead == VOID)
	match(VOID);

    else {
//	Type new_t;
//	new_t = t;
	parameter();	
	
				
	while (lookahead == ',') {
	    match(',');
	    parameter();
	    
	}
    }
    
}


/*
 * Function:	globalDeclarator
 *
 * Description:	Parse a declarator, which in Simple C is either a scalar
 *		variable, an array, or a function, all with optional
 *		pointer	declarators, or a callback (i.e., a simple function
 *		pointer).
 *
 *		global-declarator:
 *		  pointers identifier
 *		  pointers identifier ( )
 *		  pointers identifier [ num ]
 *		  pointers ( * identifier ) ( )
 */

static void globalDeclarator(const string &typespec)
{
cout << "glodecc\n";    
	unsigned indirection = pointers();
	if (lookahead == '(') {
		match('(');
		match('*');
		string name = identifier();
check.checkIfStructure(name, Type(CALLBACK, typespec, indirection));

		check.declareVariable(name, Type(CALLBACK, typespec, indirection));
		match(')');
		match('(');
		match(')');
		cout << "( CALLBACK)" << endl;
	}
	else {
		string name = identifier();
		if (lookahead == '(') {
			match('(');
			match(')');
			check.declareVariable(name, Type(FUNCTION, typespec, indirection));
			cout << "( FUNCTION)" << endl;

		} else if (lookahead == '[') {
			match('[');
			string num = number();
			match(']');
			check.declareVariable(name, Type(ARRAY, typespec, indirection));
			cout << "( ARRAY)" << endl;
		}
		else {
			check.declareVariable(name, Type(SCALAR, typespec, indirection));
			cout << "( SCALAR)" << endl;
		}
	}
}



/*
 * Function:	remainingDeclarators
 *
 * Description:	Parse any remaining global declarators after the first.
 *
 * 		remaining-declarators:
 * 		  ;
 * 		  , global-declarator remaining-declarators
 */

static void remainingDeclarators(const string& typespec)
{
    while (lookahead == ',') {
	match(',');
	globalDeclarator(typespec);
    }

    match(';');
}


/*
 * Function:	globalOrFunction
 *
 * Description:	Parse a global declaration or function definition.
 *
 * 		global-or-function:

 * 		  specifier pointers identifier ( ) remaining-decls 
 * 		  specifier pointers identifier [ num ] remaining-decls
 * 		  specifier pointers identifier ( parameters ) { ... }
 */

static void globalOrFunction()
{
cout << "global\n";
	//check.openScope();
	string typespec = specifier();
	if (typespec != "int" && typespec != "char" && lookahead == '{') {
		check.openStruct(typespec);

		match('{');
//	check.openStruct(typespec);
	
	declaration();
	declarations();
	check.closeStruct(typespec);
	match('}');
	match(';');
	}
		
	else {
	
	unsigned indirection = pointers();
	if (lookahead == '(') {
	    match('(');
	    match('*');
	    string name = identifier();
	    match(')');
	    match('(');
	    match(')');
	    check.checkIfStructure(name, Type(CALLBACK, typespec, indirection));
	    check.declareVariable(name, Type(CALLBACK, typespec, indirection));
            cout << "(" << typespec << ", CALLBACK)" << endl;
	    remainingDeclarators(typespec);

	} else {
		string name = identifier();

 //       if (typespec != "int" && typespec != "char" && lookahead != '{') {
         //       string name = identifier();
//cout << name;
              check.checkIfStructure(name, Type(SCALAR, typespec, indirection));
	if (lookahead == '[') {
		match('[');
		string num = number();
		match(']');
		Type t = Type (ARRAY, typespec, indirection);
		t.length(stoi(num));
		check.declareVariable(name, t);
		remainingDeclarators(typespec);

	    } else if (lookahead == '(') {
		match('(');
		Type t = Type(FUNCTION, typespec, indirection);
		if (lookahead == ')') {
		    match(')');
		    check.declareFunc(name, t);
		    check.checkIfStructure(name, t);
		    remainingDeclarators(typespec);

		} else {
		    check.declareFunc(name, t);
		    check.openScope();
		    parameters(t);
		    match(')');
		    match('{');
		    declarations();
		    statements();
		    check.closeScope();
			
			match('}');
		}

	    } else {
cout << "else of global\n";
		check.declareVariable(name, Type(SCALAR, typespec, indirection));
		remainingDeclarators(typespec);
		//closeScope();
		//
	}
	}
    }
	//check.closeScope();
}


/*
 * Function:	main
 *
 * Description:	Analyze the standard input stream.
 */

/*
int main()
{
	openScope();
	lookahead = lexan(buf);
	closeScope();
}
*/

int main()
{
	check.openScope();
    lookahead = lexan(lexbuf);

    while (lookahead != DONE)
	globalOrFunction();
	check.closeScope();

    exit(EXIT_SUCCESS);
}
