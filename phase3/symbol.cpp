# include "symbol.h"

Symbol::Symbol() {
	_name = "error";
}
Symbol::Symbol(const string& name, const Type& type) {
	_name = name;
	_type = type;
}
