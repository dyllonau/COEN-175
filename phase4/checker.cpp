/*
 * File:	checker.cpp
 *
 * Description:	This file contains the public and private function and
 *		variable definitions for the semantic checker for Simple C.
 *
 *		Extra functionality:
 *		- inserting an undeclared symbol with the error type
 */

# include <map>
# include <set>
# include <iostream>
# include "lexer.h"
# include "checker.h"
# include "Symbol.h"
# include "Scope.h"
# include "Type.h"


using namespace std;

static set<string> functions;
static map<string,Scope *> fields;
static Scope *outermost, *toplevel;
static const Type error;

static string undeclared = "'%s' undeclared";
static string redefined = "redefinition of '%s'";
static string redeclared = "redeclaration of '%s'";
static string conflicting = "conflicting types for '%s'";
static string incomplete = "'%s' has incomplete type";
static string nonpointer = "pointer type required for '%s'";

static Scalar integer("int");
static string invalid_operands = "invalid operands to binary %s";
static string invalid_operands2 = "invalid operand to unary %s";
static string e1 = "invalid return type";
static string e2 = "invalid type for test expression";
static string e3 = "lvalue required in expression";
static string e7 = "called object is not a function";
static string e8 = "invalid arguments to called function";
static string e9 = "using pointer to incomplete type";


# define isStructure(t) (t.isStruct() && t.indirection() == 0)

//Symbol* symbol = fields[left.specifier()]->find(id);

/*
 * Function:	openScope
 *
 * Description:	Create a scope and make it the new top-level scope.
 */

Scope *openScope()
{
    toplevel = new Scope(toplevel);

    if (outermost == nullptr)
	outermost = toplevel;

    return toplevel;
}


/*
 * Function:	closeScope
 *
 * Description:	Remove the top-level scope, and make its enclosing scope
 *		the new top-level scope.
 */

Scope *closeScope()
{
    Scope *old = toplevel;

    toplevel = toplevel->enclosing();
    return old;
}


/*
 * Function:	openStruct
 *
 * Description:	Open a scope for a structure with the specified name.  If a
 *		structure with the same name is already defined, delete it.
 */

void openStruct(const string &name)
{
    if (fields.count(name) > 0) {
	delete fields[name];
	fields.erase(name);
	report(redefined, name);
    }

    openScope();
}


/*
 * Function:	closeStruct
 *
 * Description:	Close the scope for the structure with the specified name.
 */

void closeStruct(const string &name)
{
    fields[name] = closeScope();
}


/*
 * Function:	declareSymbol
 *
 * Description:	Declare a symbol with the specified NAME and TYPE.  Any
 *		erroneous redeclaration is discarded.  If a declaration has
 *		multiple errors, only the first error is reported.  To
 *		report multiple errors, remove the "return" statements and,
 *		if desired, the final "else".
 */

void declareSymbol(const string &name, const Type &type, bool isParameter)
{
    Symbol *symbol = toplevel->find(name);

    if (symbol == nullptr)
	toplevel->insert(new Symbol(name, type));
    else if (toplevel != outermost) {
	report(redeclared, name);
	return;
    } else if (type != symbol->type()) {
	report(conflicting, name);
    	return;
    }

    if (isStructure(type)) {
	if (isParameter || type.isCallback() || type.isFunction())
	    report(nonpointer, name);
	else if (fields.count(type.specifier()) == 0)
	    report(incomplete, name);
    }
}


/*
 * Function:	defineFunction
 *
 * Description:	Define a function with the specified NAME and TYPE.  A
 *		function is always defined in the outermost scope.  This
 *		definition always replaces any previous definition or
 *		declaration.  In the case of multiple errors, only the
 *		first error is reported.  To report multiple errors, remove
 *		the "else"s.
 */

Symbol *defineFunction(const string &name, const Type &type)
{
    Symbol *symbol = outermost->find(name);

    if (functions.count(name) > 0)
	report(redefined, name);
    else if (symbol != nullptr && type != symbol->type())
	report(conflicting, name);
    else if (isStructure(type))
	report(nonpointer, name);

    outermost->remove(name);
    delete symbol;

    symbol = new Symbol(name, type);
    outermost->insert(symbol);

    functions.insert(name);
    return symbol;
}


/*
 * Function:	checkIdentifier
 *
 * Description:	Check if NAME is declared.  If it is undeclared, then
 *		declare it as having the error type in order to eliminate
 *		future error messages.
 */

Symbol *checkIdentifier(const string &name)
{
    Symbol *symbol = toplevel->lookup(name);

    if (symbol == nullptr) {
	report(undeclared, name);
	symbol = new Symbol(name, error);
	toplevel->insert(symbol);
    }

    return symbol;
}

Type checkLogicalOr(const Type& left, const Type& right) {
if (left.isError() || right.isError())
        return error;
    Type t1 = left.promote();
    Type t2 = right.promote();
    if (t1.isValue() && t2.isValue())
        return integer;
    report(invalid_operands, "||");
    return error;
}

Type checkLogicalAnd(const Type& left, const Type& right) {
if (left.isError() || right.isError())
        return error;
    Type t1 = left.promote();
    Type t2 = right.promote();
    if (t1.isValue() && t2.isValue())
        return integer;
    report(invalid_operands, "&&");
    return error;
}

Type checkEquals(const Type& left, const Type& right) {
if (left.isError() || right.isError())
        return error;
    Type t1 = left.promote();
    Type t2 = right.promote();
    if (t1 == t2)
        return integer;
    report(invalid_operands, "==");
    return error;
}

Type checkNotEquals(const Type& left, const Type& right) {
if (left.isError() || right.isError())
        return error;
    Type t1 = left.promote();
    Type t2 = right.promote();
    if (t1 == t2)
        return integer;
    report(invalid_operands, "!=");
    return error;
}

Type checkLeq(const Type& left, const Type& right) {
if (left.isError() || right.isError())
        return error;
    Type t1 = left.promote();
    Type t2 = right.promote();
    if (t1 == t2)
        return integer;
    report(invalid_operands, "<=");
    return error;
}

Type checkGeq(const Type& left, const Type& right) {
if (left.isError() || right.isError())
        return error;
    Type t1 = left.promote();
    Type t2 = right.promote();
    if (t1 == t2)
        return integer;
    report(invalid_operands, ">=");
    return error;
}

Type checkLtn(const Type& left, const Type& right) {
if (left.isError() || right.isError())
        return error;
    Type t1 = left.promote();
    Type t2 = right.promote();
    if (t1 == t2)
        return integer;
    report(invalid_operands, "<");
    return error;
}

Type checkGtn(const Type& left, const Type& right) {
if (left.isError() || right.isError())
        return error;
    Type t1 = left.promote();
    Type t2 = right.promote();
    if (t1 == t2)
        return integer;
    report(invalid_operands, ">");
    return error;
}

Type checkAdd(const Type& left, const Type& right) { //f any operand has type “pointer to T,” where T is a structure type, then T must be complete [E9]
if (left.isError() || right.isError())
        return error;
    Type t1 = left.promote();
    Type t2 = right.promote();
    if (t1 == integer && t2 == integer)
        return integer;
    else if (t1.indirection() != 0 && t2 == integer)
        return Scalar(left.specifier(), t1.indirection());
    else if (t1 == integer && t2.indirection() != 0)
       return Scalar(right.specifier(), t2.indirection());
    report(invalid_operands, "+");
    return error;
}

Type checkSub(const Type& left, const Type& right) {
if (left.isError() || right.isError())
	return error;
    Type t1 = left.promote();
    Type t2 = right.promote();
    if (t1 == integer && t2 == integer)
        return integer;
    else if (t1.indirection() != 0 && t2 == integer)
        return Scalar(left.specifier(), t1.indirection());
    else if (t1.indirection() != 0 && t2.indirection() != 0 && t1 == t2)
        return integer;
    report(invalid_operands, "-");
    return error;
}

Type checkMultiply(const Type& left, const Type& right) {
if (left.isError() || right.isError())
        return error;
    Type t1 = left.promote();
    Type t2 = right.promote();
    if (t1 == integer && t2 == integer)
        return integer;
    report(invalid_operands, "*");
    return error;
}

Type checkDivide(const Type& left, const Type& right) {
if (left.isError() || right.isError())
        return error;
    Type t1 = left.promote();
    Type t2 = right.promote();
    if (t1 == integer && t2 == integer)
        return integer;
    report(invalid_operands, "/");
    return error;
}

Type checkRem(const Type& left, const Type& right) {
if (left.isError() || right.isError())
        return error;
    Type t1 = left.promote();
    Type t2 = right.promote();
    if (t1 == integer && t2 == integer)
        return integer;
    report(invalid_operands, "%");
    return error;
}

// PREFIX

Type checkNeg(const Type& left) {
    Type t1 = left.promote();
    if (t1 == integer)
        return integer;
    report(invalid_operands2, "-");
    return error;
}

Type checkNot(const Type& left) {
    if (left.isValue())
        return integer;
    report(invalid_operands2, "!");
    return error;
}

Type checkAmpersand(const Type& left, bool lvalue) {
    if (!left.isCallback())
        return Scalar("pointer", left.indirection() + 1);
    else if (!lvalue)
        report(e3, "&");
    else
        report(invalid_operands2, "callback returning T");
    return error;
}

Type checkDeref(const Type& left) {
    Type t1 = left.promote();
    if (t1.isPointer()) {
        if (t1.isStruct())
            return left;
        return left;
    } else
        report(invalid_operands2, "*");
    return error;
}

Type checkSizeof(const Type& left) {
    if (!left.isFunction() || left.isStruct())
        return integer;
    report(invalid_operands2, "sizeof");
    return error;
}

Type checkTypecast(const Type& left) {
    if (!left.isFunction())
        return Scalar(left.specifier());
    else if (left.isPointer())
        return Scalar(left.specifier(), left.indirection());
    report("invalid operand in cast expression");
    return error;
}

//POSTFIX

Type checkArray(const Type& left, const Type& right) {
    Type t1 = left.promote();
    Type t2 = right.promote();
    if ((t1.isPointer() || t1.isStruct()) && t2 == integer)
        return left;
    report(invalid_operands, "[]");
    return error;
}
/*
Type checkFunc(const Type& left, const Type& right) {
    Type t1 = left.promote();
    Type t2 = right.promote();
    if ((t1.isFunction() || t1.isCallback()))
        return left;
    report(invalid_operands, "");
    return error;
}
*/
Type checkFunc(const Type& left, const Type& right) {
    Type t1 = left.promote();
    Type t2 = right.promote();
    if (left.isError())
        return error;
    else if (!t1.isFunction() && !t1.isCallback()) {
        report(e7);
        return error;
    }
    for (int i = 0; i < t1.parameters()->size(); i++) {
        if (t1.parameters()->size() == 0/*!t1.parameters()[i]->isValue()*/) {
            report(e8);
            return error;
        }
    }

    if ((t1.isFunction() || t1.isCallback()))
        return left;
    
    return error;
}


Type checkDirect(const Type& left, string id, bool& lvalue, const Type& expr) {
    if (left.isStruct()) {
        Symbol* symbol = fields[left.specifier()]->find(id);
        if (symbol != nullptr && !symbol->type().isArray() && lvalue) {
            lvalue = true;
            return left;
        }
    }
    report(invalid_operands, ".");
    return error;
}

Type checkIndirect(const Type& left, string id, bool& lvalue, const Type& expr) {
    Type t1 = left.promote();
    if (t1.isStruct() && t1.isPointer()) {
        Symbol* symbol = fields[left.specifier()]->find(id);
        if (symbol != nullptr && !symbol->type().isArray() && lvalue) {
            lvalue = true;
            return left;
        }
    }
    report(invalid_operands, "->");
    return error;
}

Type checkAssignment(const Type& left, const Type& right, bool& lvalue) {
    if (lvalue) {
        if (left.isCompatibleWith(right) && (left.isArray() || left.isPointer() || left.isCallback()))
            return left;
        else 
            report(invalid_operands, "=");
    }
    else 
        report(e3);
    return error;
}

Type checkReturn(const Type& left, const Type& right) {
    if (left.isCompatibleWith(right))
        return right;
    report(e1);
    return error;
}

Type checkConditions(const Type& left) {
    if (left.isValue())
        return left;
    report(e2);
    return error;
}
