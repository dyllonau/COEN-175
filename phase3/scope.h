# ifndef SCOPE_H
# define SCOPE_H
# include <string>
# include <vector>
# include "symbol.h"

class Scope {
typedef std::vector<Symbol *> Symbols;

//class Scope {
	typedef std::string string;
	Scope *_enclosing; //scope that has vector of symbols are linked together
	Symbols _symbols; //vector of symbols
public:
	Scope(Scope* enclosing = nullptr);

	Scope* enclosing() const;
	const Symbols &symbols() const;

	void insert(Symbol* symbol);
	Symbol *find(const string &name) const;
	Symbol *lookup(const string& name) const;
	void remove(const string &name);
};

# endif /* SCOPE_H */
