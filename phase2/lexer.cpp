/*
 * File:	lexer.cpp
 *
 * Description:	This file contains the public and private function and
 *		variable definitions for the lexical analyzer for Simple C.
 *
 *		Extra functionality:
 *		- checking for out of range integer literals
 *		- checking for invalid string constants
 *		- checking for invalid character constants
 */

# include <set>
# include <cstdio>
# include <cerrno>
# include <cctype>
# include <cstdlib>
# include <iostream>
# include "string.h"
# include "lexer.h"
# include "tokens.h"
# include <map>


using namespace std;
int numerrors, lineno = 1;


/* Later, we will associate token values with each keyword */

static set<string> keywords = {
    "auto",
    "break",
    "case",
    "char",
    "const",
    "continue",
    "default",
    "do",
    "double",
    "else",
    "enum",
    "extern",
    "float",
    "for",
    "goto",
    "if",
    "int",
    "long",
    "register",
    "return",
    "short",
    "signed",
    "sizeof",
    "static",
    "struct",
    "switch",
    "typedef",
    "union",
    "unsigned",
    "void",
    "volatile",
    "while",
};

map<string, int> maptoenum = {
	{ "auto", AUTO},
	{ "break", BREAK},
	{"case", CASE},
	{"char", CHAR},
	{"const", CONST},
	{"continue", CONTINUE},
	{"default", DEFAULT},
	{"do", DO},
	{"double", DOUBLE},
	{"else", ELSE},
	{"enum", ENUM},
	{"extern", EXTERN},
	{"float", FLOAT},
	{"for", FOR},
	{"goto", GOTO},
	{"if", IF},
	{"int", INT},
	{"long", LONG},
	{"register", REGISTER},
	{"return", RETURN},
	{"short", SHORT},
	{"signed", SIGNED},
	{"sizeof", SIZEOF},
	{"static", STATIC},
	{"struct", STRUCT},
	{"switch", SWITCH},
	{"typedef", TYPEDEF},
	{"union", UNION},
	{"unsigned", UNSIGNED},
	{"void", VOID},
	{"volatile", VOLATILE},
	{"while", WHILE}
};


/*
 * Function:	report
 *
 * Description:	Report an error to the standard error prefixed with the
 *		line number.  We'll be using this a lot later with an
 *		optional string argument, but C++'s stupid streams don't do
 *		positional arguments, so we actually resort to snprintf.
 *		You just can't beat C for doing things down and dirty.
 */

void report(const string &str, const string &arg)
{
    char buf[1000];

    snprintf(buf, sizeof(buf), str.c_str(), arg.c_str());
    cerr << "line " << lineno << ": " << buf << endl;
    numerrors ++;
}


/*
 * Function:	lexan
 *
 * Description:	Read and tokenize the standard input stream.  The lexeme is
 *		stored in a buffer.
 */

int lexan(string &lexbuf)
{
    static int c = cin.get();
    bool invalid, overflow;
    long val;
    int p;


    /* The invariant here is that the next character has already been read
       and is ready to be classified.  In this way, we eliminate having to
       push back characters onto the stream, merely to read them again. */

    while (!cin.eof()) {
	lexbuf.clear();


	/* Ignore white space */

	while (isspace(c)) {
	    if (c == '\n')
		lineno ++;

	    c = cin.get();
	}


	/* Check for an identifier or a keyword */

	if (isalpha(c) || c == '_') {
	    do {
		lexbuf += c;
		c = cin.get();
	    } while (isalnum(c) || c == '_');

	    if (keywords.count(lexbuf) > 0)
		cout << "";
		else {
			//cout << "identifier:" << lexbuf << endl;

			return ID;
	}	
//cout << maptoenum.find(lexbuf) << endl;
		return maptoenum.find(lexbuf)->second;


	/* Check for a number */

	} else if (isdigit(c)) {
	    do {
		lexbuf += c;
		c = cin.get();
	    } while (isdigit(c));

	    errno = 0;
	    val = strtol(lexbuf.c_str(), NULL, 0);

	    if (errno != 0 || val != (int) val)
		report("integer constant too large");

	    //cout << "number:" << lexbuf << endl;
	    return NUM;


	/* There must be an easier way to do this.  It might seem stupid at
	   this point to recognize each token separately, but eventually
	   we'll be returning separate token values to the parser, so we
	   might as well do it now. */

	} else {
	    lexbuf += c;

	    switch(c) {


	    /* Check for '|' and '||' */

	    case '|':
		c = cin.get();

		if (c == '|') {
		    lexbuf += c;
		    c = cin.get();
		}

	//	cout << "operator:" << lexbuf << endl;
		return OR;


	    /* Check for '=' and '==' */

	    case '=':
		c = cin.get();

		if (c == '=') {
		    lexbuf += c;
		    c = cin.get();
			return EQL;
		}

	//	cout << "operator:" << lexbuf << endl;
		return ASSIGN;


	    /* Check for '&' and '&&' */

	    case '&':
		c = cin.get();

		if (c == '&') {
		    lexbuf += c;
		    c = cin.get();
			return AND;
		}

	//	cout << "operator:" << lexbuf << endl;
		return ADDR;


	    /* Check for '!' and '!=' */

	    case '!':
		c = cin.get();

		if (c == '=') {
		    lexbuf += c;
		    c = cin.get();
			return NEQ;
		}

	//	cout << "operator:" << lexbuf << endl;
		return NOT;


	    /* Check for '<' and '<=' */

	    case '<':
		c = cin.get();

		if (c == '=') {
		    lexbuf += c;
		    c = cin.get();
			return LEQ;
		}

	//	cout << "operator:" << lexbuf << endl;
		return LTN;


	    /* Check for '>' and '>=' */

	    case '>':
		c = cin.get();

		if (c == '=') {
		    lexbuf += c;
		    c = cin.get();
			return GEQ;
		}

	//	cout << "operator:" << lexbuf << endl;
		return GTN;


	    /* Check for '-', '--', and '->' */

	    case '-':
		c = cin.get();

		if (c == '-') {
		    lexbuf += c;
		    c = cin.get();
			return DEC;
		} else if (c == '>') {
		    lexbuf += c;
		    c = cin.get();
			return ARROW;
		}

	//	cout << "operator:" << lexbuf << endl;
		return MINUS;


	    /* Check for '+' and '++' */

	    case '+':
		c = cin.get();

		if (c == '+') {
		    lexbuf += c;
		    c = cin.get();
			return INC;
		}

	//	cout << "operator:" << lexbuf << endl;
		return PLUS;


	    /* Check for simple, single character tokens */

	    case '*':
		c = cin.get();
	//	cout << "op:" << lexbuf << endl;
		return STAR;

		case '%':
			c = cin.get();
	//		cout << "op:" << lexbuf << endl;
			return REM;

		case ':':
			c = cin.get();
	//		cout << "op:" << lexbuf << endl;
			return COLON; 
		
		case ';':
			c = cin.get();
	//		cout << "op:" << lexbuf << endl;
			return SEMI;
	    
		case '(': 
			c = cin.get();
	//		cout << "op:" << lexbuf << endl;
			return LPAREN;
		case ')':
			c = cin.get();
	//		cout << "op:" << lexbuf << endl;
			return RPAREN;
		case '[': 
			c = cin.get();
	//		cout << "op:" << lexbuf << endl;
			return LBRACK; 
		case ']':
			c = cin.get();
	//		cout << "op:" << lexbuf << endl;
			return RBRACK;
	    case '{': 
			c = cin.get();
	//		cout << "op:" << lexbuf << endl;
			return LBRACE;
		case '}': 
			c = cin.get();
	//		cout << "op:" << lexbuf << endl;
			return RBRACE;
		case '.': 
			c = cin.get();
	//		cout << "op:" << lexbuf << endl;
			return DOT;
		case ',':
			c = cin.get();
	//		cout << "op:" << lexbuf << endl;
			return COMMA;



	    /* Check for '/' or a comment */

	    case '/':
		c = cin.get();

		if (c == '*') {
		    do {
			while (c != '*' && !cin.eof()) {
			    if (c == '\n')
				lineno ++;

			    c = cin.get();
			}

			c = cin.get();
		    } while (c != '/' && !cin.eof());

		    c = cin.get();
		    break;

		} else {
	//	    cout << "operator:" << DIV << endl;
		    return DIV;
		}


	    /* Check for a string literal */

	    case '"':
		do {
		    p = c;
		    c = cin.get();
		    lexbuf += c;

		    if (c == '\n')
			lineno ++;

		} while (p == '\\' || (c != '"' && c != '\n' && !cin.eof()));

		if (c == '\n' || cin.eof())
		    report("premature end of string constant");
		else {
		    string s = lexbuf.substr(1, lexbuf.size() - 2);
		    s = parseString(s, invalid, overflow);

		    if (invalid)
			report("unknown escape sequence");
		    else if (overflow)
			report("escape sequence out of range");
		}

		c = cin.get();
	//	cout << "string:" << lexbuf << endl;
		return STRING;


	    /* Check for a character literal */

	    case '\'':
		do {
		    p = c;
		    c = cin.get();
		    lexbuf += c;

		    if (c == '\n')
			lineno ++;

		} while (p == '\\' || (c != '\'' && c != '\n' && !cin.eof()));

		if (c == '\n' || cin.eof())
		    report("premature end of character constant");
		else {
		    string s = lexbuf.substr(1, lexbuf.size() - 2);
		    s = parseString(s, invalid, overflow);

		    if (invalid)
			report("unknown escape sequence");
		    else if (overflow)
			report("escape sequence out of range");
		    else if (s.size() == 0)
			report("empty character constant");
		    else if (s.size() != 1)
			report("multi-character character constant");
		}

		c = cin.get();
	//	cout << "character:" << lexbuf << endl;
		return CHARACTER;


	    /* Handle EOF here as well */

	    case EOF:
		return DONE;


	    /* Everything else is illegal */

	    default:
		c = cin.get();
		break;
	    }
	}
    }

    return ILLEGAL;
}


/*
 * Function:	main
 *
 * Description:	Read and tokenize and standard input stream.
 *
/

int main()
{
    string lexbuf;

    while (lexan(lexbuf)) {
	continue;
}

    return 0;
}
*/
