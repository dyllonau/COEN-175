# include <string>
# include <vector>
# include <set>
# include "scope.h"
# include "lexer.h"
# include <iostream>
#include "checker.h"

/*
- declareFunc()
- defineFunc()
- checkIfStructure()
- checkID()
*/

using namespace std;

static set<string> structures;
static Scope* current = nullptr;

static string undeclared = "'%s' undeclared";
static string redefined = "redefinition of '%s'";
static string redeclared = "redeclaration of '%s'";
static string conflicting = "conflicting types for '%s'";
static string pointerneeded = "pointer type required for '%s'";
static string incomplete = "'%s' has incomplete type";


void Checker::openScope() {
	Scope* newScope = new Scope(current);
	current = newScope;
	//string type = specifier();
	cout << "checker open scope " << endl;
}

void Checker::closeScope() {
	current = current->enclosing();
/*
	Scope* temp = current;
	current = current->enclosing();
	temp->enclosing() = nullptr;
	free(temp);
*/
	cout << "checker close scope\n";
}


void Checker::openStruct(const string &name) {
	if (structures.count(name) > 0)
		report(redefined, name);
	openScope();
//	cout << "declare as " << type << endl;
}

void Checker::closeStruct(const string &name) {
	closeScope();
	structures.insert(name);
}

void Checker::declareFunc(const string& name, const Type& type) {
	Symbol* symbol = current->find(name);
	if (symbol == nullptr) {
cout << name << " big funcc\n";
		symbol = new Symbol(name, type);
		current->insert(symbol);
	}
	else
                defineFunc(name, type);
}

void Checker::defineFunc(const string& name, const Type& type) {
	Symbol* symbol = current->find(name);
	if (type != symbol->type())
		report(conflicting, name);
	else if (type.parameters() == symbol->type().parameters())
		report(redefined, name);
}

void Checker::declareVariable(const string& name, const Type& type) {
	Symbol* symbol = current->find(name);
	if (symbol == nullptr) {
cout << "bruh" << name;
		symbol = new Symbol(name, type);
		current->insert(symbol);
	} else if (current->enclosing() != nullptr)
		report(redeclared, name);
	else if (type != symbol->type())
		report(conflicting, name);
	

}

void Checker::checkIfStructure(const string& name, const Type& type) {
        if (type.isStruct() && type.indirection() == 0 && structures.count(type.specifier()) == 0)
                report(incomplete, name);
	
	else if (type.isStruct() && type.indirection() == 0 && (type.isCallback() || type.isFunction()))
		report(pointerneeded, name);
	//else if (type.isStruct() && type.indirection() == 0 && structures.count(type.specifier()) == 0)
	//	report(incomplete, name);
}

void Checker::checkID(const string& name) {
	Symbol* symbol = current->lookup(name);
	if (symbol == nullptr) {
		cout << "unfound\n";
		report(undeclared, name);
	}	
	else
		cout << "found\n";
}
