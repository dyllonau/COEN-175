# ifndef CHECKER_H
# define CHECKER_H

# include <string>
# include <vector>
# include "scope.h"
# include "lexer.h"
# include <set>
using namespace std;
class Checker {
public:
	void openScope();
	void closeScope();
	void openStruct(const string& name);
	void closeStruct(const string& name);
	void declareFunc(const string& name, const Type& type);
	void defineFunc(const string& name, const Type& type);
	void declareVariable(const string& name, const Type& type);
	void checkIfStructure(const string& name, const Type& type);
	void checkID(const string& name);
};

#endif
