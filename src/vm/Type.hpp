#ifndef TYPE_HPP
#define TYPE_HPP

#include <vector>
#include <iostream>

namespace ls {

enum class Nature {
	UNKNOWN, VALUE, POINTER, VOID
};

class BaseRawType {
public:
	virtual const std::string getName() const { return "?"; }
	virtual const std::string getClass() const { return "?"; }
	virtual const std::string getJsonName() const { return "?"; }
};

class VoidRawType : public BaseRawType {
public:
	virtual const std::string getName() const { return "void"; }
	virtual const std::string getJsonName() const { return "void"; }
};

class NullRawType : public BaseRawType {
public:
	virtual const std::string getName() const { return "null"; }
	virtual const std::string getClass() const { return "Null"; }
	virtual const std::string getJsonName() const { return "null"; }
};

class BooleanRawType : public BaseRawType {
public:
	virtual const std::string getName() const { return "bool"; }
	virtual const std::string getClass() const { return "Boolean"; }
	virtual const std::string getJsonName() const { return "boolean"; }
};

class NumberRawType : public BaseRawType {
public:
	virtual const std::string getName() const { return "number"; }
	virtual const std::string getClass() const { return "Number"; }
	virtual const std::string getJsonName() const { return "number"; }
};

class IntegerRawType : public NumberRawType {
public:
	virtual const std::string getName() const { return "int"; }
	virtual const std::string getClass() const { return "Number"; }
	virtual const std::string getJsonName() const { return "number"; }
};

class LongRawType : public NumberRawType {
public:
	virtual const std::string getName() const { return "long"; }
	virtual const std::string getClass() const { return "Number"; }
	virtual const std::string getJsonName() const { return "number"; }
};

class FloatRawType : public NumberRawType {
public:
	virtual const std::string getName() const { return "real"; }
	virtual const std::string getClass() const { return "Number"; }
	virtual const std::string getJsonName() const { return "number"; }
};

class StringRawType : public BaseRawType {
public:
	virtual const std::string getName() const { return "string"; }
	virtual const std::string getClass() const { return "String"; }
	virtual const std::string getJsonName() const { return "string"; }
};

class ArrayRawType : public BaseRawType {
public:
	virtual const std::string getName() const { return "array"; }
	virtual const std::string getClass() const { return "Array"; }
	virtual const std::string getJsonName() const { return "array"; }
};

class MapRawType : public BaseRawType {
public:
	virtual const std::string getName() const { return "map"; }
	virtual const std::string getClass() const { return "Map"; }
	virtual const std::string getJsonName() const { return "map"; }
};

class IntervalRawType : public BaseRawType {
public:
	virtual const std::string getName() const { return "interval"; }
	virtual const std::string getClass() const { return "Array"; }
	virtual const std::string getJsonName() const { return "array"; }
};

class ObjectRawType : public BaseRawType {
public:
	virtual const std::string getName() const { return "object"; }
	virtual const std::string getClass() const { return "Object"; }
	virtual const std::string getJsonName() const { return "object"; }
};

class FunctionRawType : public BaseRawType {
public:
	virtual const std::string getName() const { return "function"; }
	virtual const std::string getClass() const { return "Function"; }
	virtual const std::string getJsonName() const { return "function"; }
};

class ClassRawType : public BaseRawType {
public:
	virtual const std::string getName() const { return "class"; }
	virtual const std::string getClass() const { return "Class"; }
	virtual const std::string getJsonName() const { return "class"; }
};

class RawType {
public:
	static const BaseRawType* const UNKNOWN;
	static const VoidRawType* const VOID;
	static const NullRawType* const NULLL;
	static const BooleanRawType* const BOOLEAN;
	static const NumberRawType* const NUMBER;
	static const IntegerRawType* const INTEGER;
	static const LongRawType* const LONG;
	static const FloatRawType* const FLOAT;
	static const StringRawType* const STRING;
	static const ArrayRawType* const ARRAY;
	static const MapRawType* const MAP;
	static const IntervalRawType* const INTERVAL;
	static const ObjectRawType* const OBJECT;
	static const FunctionRawType* const FUNCTION;
	static const ClassRawType* const CLASS;
};

class Type {
public:

	const BaseRawType* raw_type;
	Nature nature;
	bool native; // A C++ objects, memory management is done outside the language
	std::string clazz;
	std::vector<Type> element_types;
	std::vector<Type> return_types;
	std::vector<Type> arguments_types;

	Type();
	Type(const BaseRawType* raw_type, Nature nature, bool native = false);
	Type(const BaseRawType* raw_type, Nature nature, const Type& elements_type, bool native = false);
	Type(const BaseRawType* raw_type, Nature nature, const std::vector<Type>& element_types, bool native = false);

	bool must_manage_memory() const;

	Type getReturnType() const;
	void setReturnType(Type type);

	void addArgumentType(Type type);
	void setArgumentType(size_t index, Type type);
	const Type getArgumentType(size_t index) const;
	const std::vector<Type>& getArgumentTypes() const;

	const Type getElementType(size_t i = 0) const;
	void setElementType(Type);

	bool will_take(const std::vector<Type>& args_type);
	bool will_take_element(const Type& arg_type);
	Type mix(const Type& x) const;

	void toJson(std::ostream&) const;

	bool isNumber() const;

	bool operator ==(const Type& type) const;
	inline bool operator !=(const Type& type) const { return !(*this == type); }

	bool compatible(const Type& type) const;

	/*
	 * Static part
	 */
	static const Type VOID;
	static const Type VALUE;
	static const Type POINTER;

	static const Type UNKNOWN;
	static const Type NULLL;
	static const Type BOOLEAN;
	static const Type BOOLEAN_P;
	static const Type NUMBER;
	static const Type INTEGER;
	static const Type INTEGER_P;
	static const Type LONG;
	static const Type FLOAT;
	static const Type FLOAT_P;
	static const Type STRING;
	static const Type OBJECT;
	static const Type ARRAY;
	static const Type PTR_ARRAY;
	static const Type INT_ARRAY;
	static const Type FLOAT_ARRAY;
	static const Type STRING_ARRAY;
	static const Type MAP;
	static const Type PTR_PTR_MAP;
	static const Type PTR_INT_MAP;
	static const Type PTR_FLOAT_MAP;
	static const Type INT_PTR_MAP;
	static const Type INT_INT_MAP;
	static const Type INT_FLOAT_MAP;
	static const Type INTERVAL;
	static const Type FUNCTION;
	static const Type FUNCTION_P;
	static const Type CLASS;

	static bool list_compatible(const std::vector<Type>& expected, const std::vector<Type>& actual);
	static bool list_more_specific(const std::vector<Type>& old, const std::vector<Type>& neww);
	static bool more_specific(const Type& old, const Type& neww);
	static Type get_compatible_type(const Type& t1, const Type& t2);
	static std::string get_nature_name(const Nature& nature);
	static std::string get_nature_symbol(const Nature& nature);
};

std::ostream& operator << (std::ostream&, const Type&);

}

#endif
