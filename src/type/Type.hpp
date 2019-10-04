#ifndef TYPE_HPP
#define TYPE_HPP

#include <vector>
#include <map>
#include <set>
#include <iostream>
#include <cassert>
#include "../constants.h"
#if COMPILER
#include "llvm/IR/Type.h"
#include "llvm/IR/DerivedTypes.h"
#endif

namespace ls {

class Type;
class Function;
class Value;
class Function_type;
class Environment;
#if COMPILER
class Compiler;
#endif

class Type {
public:

	Type(Environment& env, bool native = false);
	Type(const Type* raw_type, bool native = false);
	Type(std::set<const Type*>, const Type* folded);
	virtual ~Type() {}

	Environment& env;
	const Type* folded;
	bool native = false; // A C++ object, memory management is done outside the language
	bool temporary = false;
	bool constant = false;
	bool reference = false;
	bool placeholder = false;

	virtual int id() const { return 0; }
	virtual const Type* return_type() const;
	virtual const Type* argument(size_t) const;
	virtual const std::vector<const Type*>& arguments() const;
	virtual const Type* element() const;
	virtual const Type* key() const;
	virtual const Type* member(int) const;
	virtual const Value* function() const { return nullptr; }

	void toJson(std::ostream&) const;
	virtual const std::string getJsonName() const = 0;
	virtual std::string to_string() const;
	virtual std::string class_name() const = 0;
	virtual const std::string getName() const = 0;
	virtual std::ostream& print(std::ostream& os) const = 0;

	bool must_manage_memory() const;
	virtual bool iterable() const { return false; }
	virtual bool container() const { return false; }
	virtual bool callable() const { return false; }
	virtual const Type* add_temporary() const;
	virtual const Type* not_temporary() const;
	virtual const Type* add_constant() const;
	virtual const Type* not_constant() const;
	#if COMPILER
	virtual llvm::Type* llvm(Compiler& c) const = 0;
	#endif
	virtual const Type* iterator() const { assert(false); }
	const Type* pointer() const;
	virtual const Type* pointed() const { assert(false); }
	bool castable(const Type* type, bool strictCast = false) const;
	bool strictCastable(const Type* type) const;
	virtual int distance(const Type* type) const = 0;

	const Type* operator + (const Type* type) const;
	void operator += (const Type* type);
	const Type* operator * (const Type* t2) const;
	const Type* fold() const;
	virtual bool operator == (const Type*) const = 0;

	template <class T> bool is_type() const;
	template <class T> bool can_be_type() const;
	bool is_any() const;
	bool is_number() const;
	bool can_be_number() const;
	bool can_be_numeric() const;
	bool can_be_container() const;
	bool can_be_callable() const;
	bool is_integer() const;
	bool is_bool() const;
	bool can_be_bool() const;
	bool is_long() const;
	bool is_real() const;
	bool is_mpz() const;
	bool is_mpz_ptr() const;
	bool is_array() const;
	bool is_string() const;
	bool is_set() const;
	bool is_interval() const;
	bool is_map() const;
	bool is_closure() const;
	bool is_function() const;
	bool is_function_object() const;
	bool is_function_pointer() const;
	bool is_object() const;
	bool is_never() const;
	bool is_null() const;
	bool is_class() const;
	bool is_pointer() const;
	bool is_struct() const;
	bool is_polymorphic() const;
	bool is_primitive() const;
	bool is_void() const;
	bool is_template() const;

	virtual void implement(const Type*) const {}
	virtual void reset() const {}

	virtual Type* clone() const = 0;

	static unsigned int placeholder_counter;

	// Const types to be used to optimize return of references
	static const std::vector<const Type*> empty_types;

    static const Type* array(const Type*);
	static const Type* const_array(const Type*);
	static const Type* tmp_array(const Type*);
	static const Type* set(const Type*);
	static const Type* const_set(const Type*);
	static const Type* tmp_set(const Type*);
	static const Type* map(const Type*, const Type*);
	static const Type* tmp_map(const Type*, const Type*);
	static const Type* const_map(const Type*, const Type*);
    static const Type* fun();
	static const Type* fun(const Type* return_type, std::vector<const Type*> arguments = {}, const Value* function = nullptr);
	static const Type* fun_object(const Type* return_type, std::vector<const Type*> arguments = {}, const Value* function = nullptr);
	static const Type* closure(const Type* return_type, std::vector<const Type*> arguments = {}, const Value* function = nullptr);
	static const Type* structure(const std::string name, std::initializer_list<const Type*> types);
	static const Type* compound(std::vector<const Type*> types);
	static const Type* compound(std::initializer_list<const Type*> types);
	static const Type* tmp_compound(std::initializer_list<const Type*> types);

	static const Type* meta_add(const Type* t1, const Type* t2);
	static const Type* meta_mul(const Type* t1, const Type* t2);
	static const Type* meta_base_of(const Type* type, const Type* base);
	static const Type* meta_temporary(const Type* type);
	static const Type* meta_not_temporary(const Type* type);
};

std::ostream& operator << (std::ostream&, const Type*);
std::ostream& operator << (std::ostream&, const std::vector<const Type*>&);
#if COMPILER
std::ostream& operator << (std::ostream&, const llvm::Type*);
#endif

}

#endif
