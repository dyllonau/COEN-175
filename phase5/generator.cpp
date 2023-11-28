# include "Tree.h"
# include "Scope.h"
# include "Symbol.h"
# include "Type.h"
# include "generator.h"
# include <iostream>


using namespace std;

static int offset;
static ostream& operator<<(ostream& ostr, Expression* expr);

void Procedure::generate() {
	Symbols symbols = _body->declarations()->symbols();
	offset = 8;
	int bytesAllocated = 0;
	unsigned i = 0;
	 while (i < _id->type().parameters()->size()) {
			if (!symbols[i]->type().isError()) {
				symbols[i]->_offset = offset;
				offset += symbols[i]->type().size();
				++i;
			}
	}

	i = _id->type().parameters()->size();

	 while (i < symbols.size()) {
		if (!symbols[i]->type().isError()) {
			bytesAllocated = bytesAllocated - symbols[i]->type().size();
			symbols[i]->_offset = bytesAllocated;
			i++;
		}
	}

		cout << _id->name() << ":" << endl;
		cout << "\tpushl\t%ebp" << endl;
		cout << "\tmovl\t%esp,\t%ebp" << endl;
		cout << "\tsubl\t$" << offset << ",\t%esp" << endl;
		_body->generate();
		cout << "\tmovl\t%ebp,\t%esp" << endl;
		cout << "\tpopl\t%ebp" << endl;
		cout << "\tret" << endl;
}

//void Expression::generate() {

void Simple::generate() {
	_expr->generate();
}

void Block::generate() {
	for (auto stmt : _stmts)
		stmt->generate();
}

void Assignment::generate() {
	bool flag;

	if (_left != nullptr && _right != nullptr) {
		flag = true;
		_left->generate();
		_right->generate();

		if (flag)
			cout << "\tmovl\t" << _right << ",\t" << _left << endl;;
	}
}

void Call::generate() {
	int byteSize = 0;
	int count = (int)_args.size() - 1;
		while (count > -1) {
			_args[count]->generate();
			byteSize += _args[count]->type().size();
			cout << "\tpushl\t" << _args[count] << endl;
			count--;
		}
	cout << "\tcall\t" << _expr << endl;
	cout << "\taddl\t$" << byteSize << ",\t%esp" << endl;
}

static ostream& operator<<(ostream& ostr, Expression* expr) {
	if (expr) {
		expr->operand(ostr);
		return ostr;
	}
	return ostr;
}

void Identifier::operand(ostream& ostr) const {
	if (!_symbol->type().isError()) {
		if (_symbol->_offset != 0)
			ostr << _symbol->_offset << "(%ebp)";
		else
			ostr << _symbol->name();
	}
}

void Expression::operand(ostream& ostr) const {
	std::cout << "wrong";
}

void Number::operand(ostream& ostr) const {
	ostr << "$" << _value;
}

void generateGlobals(Scope* global) {
	for (auto symbol : global->symbols()) {
		if (!symbol->type().isFunction())
			cout << "\t.comm\t" << symbol->name() << ", " << symbol->type().size() << endl;
		else
			cout << "\t.globl\t" << symbol->name() << endl;
	}
}
