# include "scope.h"
# include <iostream>

Scope::Scope(Scope* enclosing) {
	_enclosing = enclosing;
}

Scope* Scope::enclosing() const {
	return _enclosing;
}

const Scope::Symbols& Scope::symbols() const {
	return _symbols;
}

void Scope::insert(Symbol* symbol) {

	_symbols.push_back(symbol);
}

Symbol* Scope::find(const string& name) const {
	
	for (unsigned i = 0; i < _symbols.size(); i++) {
		if (_symbols[i]->name() == name) {
			Symbol* symbol = _symbols[i];
			return _symbols[i];
		}
	}
	return nullptr;
}

Symbol* Scope::lookup(const string& name) const { //IMPORTANT: needs to see if declared variable is either already existing or has never been made.
	Symbol* symbol = find(name);
std::cout << "looking..\n";
	if (symbol != nullptr) //symbol is found in current scope
		return symbol;
	else { //symbol is not in current scope; looks in adjacent enclosed scope
		while (_enclosing != nullptr) 
			return _enclosing->lookup(name);
		return nullptr;
	}
}

void Scope::remove(const string& name) {
	for (unsigned i = 0; i < _symbols.size(); i++) {
		if (_symbols[i]->name() == name) {
			_symbols.erase(_symbols.begin() + i);
			break;
		}
	}
}
