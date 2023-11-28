# ifndef TYPE_H
# define TYPE_H
# include <string>
# include <vector>
# include <ostream>

enum { ARRAY, CALLBACK, ERROR, FUNCTION, SCALAR };

typedef std::vector<class Type> Parameters;

class Type {
protected:
	typedef std::string string;

	int _kind;
	string _specifier;
	unsigned _indirection;

	unsigned _length;
	Parameters *_parameters;

public:
	//enum { ARRAY, CALLBACK, ERROR, FUNCTION, SCALAR }; // local to type class; i.e., must use type::array
	Type(int kind, const string& specifier, unsigned indirection);
	Type();

	const string& specifier() const { return _specifier; }
	unsigned indirection() const { return _indirection; }

	bool isArray() const { return _kind == ARRAY; }
	bool isCallback() const { return _kind == CALLBACK; }
	bool isError() const { return _kind == ERROR; }
	bool isFunction() const { return _kind == FUNCTION; }
	bool isScalar() const { return _kind == SCALAR; }

	bool operator ==(const Type& rhs) const;
	bool operator !=(const Type& rhs) const;

	bool isStruct() const;

	unsigned length() const { return _length; }
	Parameters* parameters() const { return _parameters; }

	void length(unsigned len) { _length = len; }
	void parameters(Parameters* params) { _parameters = params; }
};

std::ostream &operator <<(std::ostream &ostr, const Type &type);

#endif /* TYPE_H */
