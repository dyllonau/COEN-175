/*
 * File:	checker.h
 *
 * Description:	This file contains the public function declarations for the
 *		semantic checker for Simple C.
 */

# ifndef CHECKER_H
# define CHECKER_H
# include <string>
# include "Scope.h"

using namespace std;

Scope *openScope();
Scope *closeScope();

void openStruct(const std::string &name);
void closeStruct(const std::string &name);

void declareSymbol(const std::string &name, const Type &type, bool = false);

Symbol *defineFunction(const std::string &name, const Type &type);
Symbol *checkIdentifier(const std::string &name);

Type checkLogicalOr(const Type& left, const Type& right);
Type checkLogicalAnd(const Type& left, const Type& right);
Type checkEquals(const Type& left, const Type& right);
Type checkNotEquals(const Type& left, const Type& right);

Type checkLeq(const Type& left, const Type& right);

Type checkGeq(const Type& left, const Type& right);

Type checkLtn(const Type& left, const Type& right);

Type checkGtn(const Type& left, const Type& right);

Type checkAdd(const Type& left, const Type& right);

Type checkSub(const Type& left, const Type& right);

Type checkMultiply(const Type& left, const Type& right);
Type checkDivide(const Type& left, const Type& right);
Type checkRem(const Type& left, const Type& right);
// PREFIX

Type checkNeg(const Type& left);
Type checkNot(const Type& left);

Type checkAmpersand(const Type& left, bool lvalue);

Type checkDeref(const Type& left);
Type checkSizeof(const Type& left);

Type checkTypecast(const Type& left);
//POSTFIX

Type checkArray(const Type& left, const Type& right);

Type checkFunc(const Type& left, const Type& right);

Type checkFunc(const Type& left, const Type& right);


Type checkDirect(const Type& left, string id, bool& lvalue, const Type& expr);

Type checkIndirect(const Type& left, string id, bool& lvalue, const Type& expr);

Type checkAssignment(const Type& left, const Type& right, bool& lvalue);

Type checkReturn(const Type& left, const Type& right);

Type checkConditions(const Type& left);

# endif /* CHECKER_H */
