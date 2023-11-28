# include <set>
# include <cstdio>
# include <cerrno>
# include <cctype>
# include <cstdlib>
# include <iostream>
# include <string>
# include "string.h"
# include "lexer.h"
# include "tokens.h"
# include <stdexcept>

using namespace std;

string lexbuf;

int lookahead = 1;

void error() {
	string s = "Syntax Error.";
	throw runtime_error(s);
}

void match(int t) {
	if (lookahead == t) {
		lookahead = lexan(lexbuf);
	}
	else {
		error();
	}
}

void orExpression() {
	cout << lookahead << "orEX\n";
	andf();
	while (1) {
		if (lookahead == OR) {

			match(OR);
			andf();
			cout << "or" << endl;
		}
		else
			break;
	}
}

void andf() {
	eql();
	while (1) {
		if (lookahead == AND) {
			match(AND);
			eql();
			cout << "and" << endl;
		}
		else
			break;
	}
}

void eql() {
	compr();
	while (1) {
		if (lookahead == EQL) {
			match(EQL);
			compr();
			cout << "eql" << endl;
		}
		else if (lookahead == NEQ) {
			match(NEQ);
			compr();
			cout << "neq" << endl;
		}
		else
			break;
	}
}

void compr() {
	expr();
	while (1) {
		if (lookahead == LEQ) {
			match(LEQ);
			expr();
			cout << "leq" << endl;
		}
		else if (lookahead == LTN) {
			match(LTN);
			expr();
			cout << "ltn" << endl;
		}
		else if (lookahead == GEQ) {
			match(GEQ);
			expr();
			cout << "geq" << endl;
		}
		else if (lookahead == GTN) {
			match(GTN);
			expr();
			cout << "gtn" << endl;
		}
		else
			break;
	}
}

void expr() {
	term();

	while (1) {
		if (lookahead == '+') {
			match('+');
			term();
			cout << "add" << endl;
		}
		else if (lookahead == '-') {
			match('-');
			term();
			cout << "sub" << endl;
		}
		else {
			break;
		}

	}

}

void term() {
	factor();
	while (1) {
		if (lookahead == '*') {
			match('*');
			factor();
		}
		else if (lookahead == '/') {
			match('/');
			factor();
			cout << "div" << endl;
		}
		else if (lookahead == '%') {
			match('%');
			factor();
			cout << "rem" << endl;
		}
		else
			break;
	}
}

void factor() {
	while (1) {
		if (lookahead == '(') {
			match('(');
			specifier();
			pointers();
			match(')');
			point();
			cout << "cast" << endl;
		}
		else if (lookahead == '&') {
			match('&');
			point();
			cout << "addr" << endl;
		}
		else if (lookahead == '*') {
			match('*');
			point();
			cout << "deref" << endl;
		}
		else if (lookahead == '!') {
			match('!');
			point();
			cout << "not" << endl;
		}
		else if (lookahead == '-') {
			match('-');
			point();
		}
		else if (lookahead == SIZEOF) {
			match(SIZEOF);
			if (lookahead == '(') {
				match('(');
				specifier();
				pointers();
				match(')');
				cout << "sizeof" << endl;
			}
			else {
				point();
				cout << "sizeof" << endl;
			}
		}
		else {
			point();
			break;
		}
	}
}

void point() {
	expression();
	while (1) {
		if (lookahead == '[') {
			match('[');
			expression();
			match(']');
			cout << "index" << endl;
		}

		else if (lookahead == '(') {
			match('(');
			if (lookahead == ')') {
				match(')');
				cout << "call" << endl;
			}
			else {
				orExpression();
				while (lookahead == ',') {
					match(',');
					orExpression();
				}
				match(')');
				cout << "call" << endl;
			}
		}
		else if (lookahead == DOT) {
			match(DOT);
			match(ID);
			cout << "dot" << endl;
		}
		else if (lookahead == ARROW) {
			match(ARROW);
			match(ID);
			cout << "arrow" << endl;

		}
		else
			break;
	}
}

void expression() {
	if (lookahead == ID)
		match(ID);
	else if (lookahead == NUM)
		match(NUM);
	else if (lookahead == STRING)
		match(STRING);
	else if (lookahead == CHARACTER)
		match(CHARACTER);
	else if (lookahead == '(') {
		match('(');
		orExpression();
		match(')');
	}
}

void assignment() {
	orExpression();
	if (lookahead == '=') {
		match('=');
		orExpression();
	}
}

void statements() {
	while (lookahead != DONE)
		statement();
}

void statement() {

	if (lookahead == IF) {
		match(IF);
		match('(');
		orExpression();
		match(')');
		statement();
		if (lookahead == ELSE) {
			match(ELSE);
			statement();
		}
	}
	else if (lookahead == '{') {
		match('{');
		declarations();
		statements();
		match('}');
	}
	else if (lookahead == RETURN) {
		match(RETURN);
		orExpression();
		match(';');
	}
	else if (lookahead == WHILE) {
		match(WHILE);
		match('(');
		orExpression();
		match(')');
		statement();
	}
	else if (lookahead == FOR) {
		match(FOR);
		match('(');
		assignment();
		match(';');
		orExpression();
		match(';');
		assignment();
		match(')');
		statement();
	}
	else {
		assignment();
		match(';');
	}
}




void transunit() {
	type_def();
	fog();
}

void type_def() {
	match(STRUCT);
	match(ID);
	if (lookahead == '{') {
		match('{');
		declaration();
		declarations();
		match('}');
	}
	match(';');
}
		

int specifier() {
	if (lookahead == CHAR) {
		match(CHAR);
		return CHAR;
	}
	else if (lookahead == INT) {
		match(INT);
		return INT;
	} else if (lookahead == STRUCT) {
		match(STRUCT);
		match(ID);
		return STRUCT;
	} else
		return 0;
}

void pointers() {
	while (lookahead == '*') 
		match('*');
}

void parameters() {
	if (lookahead == VOID)
		match(VOID);
	else {
		parameter();
		while (lookahead == ',') {
			match(',');
			parameter();
		}
	}
}

void parameter() {
	specifier();
	pointers();
	if (lookahead == ID) {
		match(ID);
	}
	else if (lookahead == '(') {
		match('(');
		match('*');
		match(ID);
		match(')');
		match('(');
		match(')');
	}
}

void declaration() {
	specifier();
	declarator();
	while (lookahead == ',') {
		match(',');
		declarator();
	}
	match(';');
}

void declarations() {
	while (lookahead != DONE && specifier())
		declaration();
}

void declarator() {
	pointers();
	if (lookahead == '(') {
		match('(');
		match('*');
		match(ID);
		match(')');
		match('(');
		match(')');
	}
	else {
	match(ID);
	if (lookahead == '[') {
		match('[');
		match(NUM);
		match(']');
	}
}
}

void fog() {
	if (specifier() == STRUCT && lookahead == '{') {
		declaration();
		declarations();
		match('}');
		match(';');
	} else {
		pointers();
		if (lookahead == '(') {
			match('(');
			match('*');
			match(ID);
			match(')');
			match('(');
			match(')');
		} else {
			match(ID);

			if (lookahead == '[') {
				match('[');
				match(NUM);
				match(']');
				remainingDeclarators();
			}
			else if (lookahead == '(') {
				match('(');
				if (lookahead == ')') {
					match(')');
					remainingDeclarators();
				}
				else {
					parameters();
					match(')');
					match('{');
					declarations();
					statements();
					match('}');
				}
			}
			else
				remainingDeclarators();
}}}

void remainingDeclarators() {
	while (lookahead == ',') {
		match(',');
		globalDeclarator();
	}
	match(';');
}

void globalDeclaration() {
	if (specifier() == STRUCT && lookahead == '{') {
		type_def();
	}
	else {
		globalDeclarator();
		while (lookahead == ',') {
			match(',');
			globalDeclarator();
		}
		match(';');
	}
}

void globalDeclarator() {
	pointers();

	if (lookahead == '(') {
		match('(');
		match('*');
		match(ID);
		match(')');
		match('(');
		match(')');
	}
	else {
		match(ID);

		if (lookahead == '[') {
			match('[');
			match(NUM);
			match(']');
		}
		else if (lookahead == '(')
			match(')');
	}
}

int main() {	
	lookahead = lexan(lexbuf);

	while (lookahead != DONE) {
		fog();
	}
exit(EXIT_SUCCESS);
}
