#ifndef TYPE_HPP
#define TYPE_HPP

#include <vector>
#include <map>
#include <set>
#include <iostream>
#include <cassert>
#include "../constants.h"
#include "llvm/IR/Type.h"
#include "llvm/IR/DerivedTypes.h"

namespace ls {

class Type;
class Function;
class Value;
class Compiler;
class Function_type;

class Type {
public:

	Type(bool native = false);
	Type(const Type* raw_type, bool native = false);
	Type(std::set<const Type*>, const Type* folded);
	virtual ~Type() {}

	const Type* folded;
	bool native = false; // A C++ object, memory management is done outside the language
	bool temporary = false;
	bool constant = false;
	bool reference = false;
	bool placeholder = false;

	virtual int id() const { return 0; }
	virtual const Type* return_type() const { return Type::any; }
	virtual const Type* argument(size_t) const { return Type::any; }
	virtual const std::vector<const Type*>& arguments() const { return Type::empty_types; }
	virtual const Type* element() const { return Type::any; }
	virtual const Type* key() const { return Type::any; }
	virtual const Type* member(int) const { return Type::any; }
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

	/*
	 * Static part
	 */
	static unsigned int placeholder_counter;

	static const Type* const void_;
	static const Type* const never;
	static const Type* const any;
	static const Type* const tmp_any;
	static const Type* const null;
	static const Type* const const_any;
	static const Type* const boolean;
	static const Type* const const_boolean;
	static const Type* const number;
	static const Type* const const_number;
	static const Type* const i8;
	static const Type* const i8_ptr;
	static const Type* const integer;
	static const Type* const const_integer;
	static const Type* const long_;
	static const Type* const const_long;
	static const Type* const mpz;
	static const Type* const tmp_mpz;
	static const Type* const const_mpz;
	static const Type* const mpz_ptr;
	static const Type* const tmp_mpz_ptr;
	static const Type* const const_mpz_ptr;
	static const Type* const tmp_string;
	static const Type* const const_string;
	static const Type* const real;
	static const Type* const const_real;
	static const Type* const string;
	static const Type* array(const Type* = Type::void_);
	static const Type* const_array(const Type* = Type::void_);
	static const Type* tmp_array(const Type* = Type::void_);
	static const Type* const object;
	static const Type* const tmp_object;
	static const Type* set(const Type* = Type::void_);
	static const Type* const_set(const Type* = Type::void_);
	static const Type* tmp_set(const Type* = Type::void_);
	static const Type* map(const Type* = Type::void_, const Type* = Type::void_);
	static const Type* tmp_map(const Type* = Type::void_, const Type* = Type::void_);
	static const Type* const_map(const Type* = Type::void_, const Type* = Type::void_);
	static const Type* const interval;
	static const Type* const const_interval;
	static const Type* const tmp_interval;
	static const Type* fun(const Type* return_type = Type::void_, std::vector<const Type*> arguments = {}, const Value* function = nullptr);
	static const Type* fun_object(const Type* return_type = Type::void_, std::vector<const Type*> arguments = {}, const Value* function = nullptr);
	static const Type* closure(const Type* return_type = Type::void_, std::vector<const Type*> arguments = {}, const Value* function = nullptr);
	static const Type* structure(const std::string name, std::initializer_list<const Type*> types);
	static const Type* clazz(const std::string name = "class?");
	static const Type* const_class(const std::string name = "class?");
	static const Type* template_(std::string name);
	static const Type* compound(std::vector<const Type*> types);
	static const Type* compound(std::initializer_list<const Type*> types);
	static const Type* tmp_compound(std::initializer_list<const Type*> types);

	static const Type* meta_add(const Type* t1, const Type* t2);
	static const Type* meta_mul(const Type* t1, const Type* t2);
	static const Type* meta_base_of(const Type* type, const Type* base);
	static const Type* meta_not_temporary(const Type* type);
	
	static const Type* generate_new_placeholder_type();
	static std::vector<const Type*> placeholder_types;
	static void clear_placeholder_types();

	// Const types to be used to optimize return of references
	static const std::vector<const Type*> empty_types;
	static std::map<std::set<const Type*>, const Type*> compound_types;
	static std::map<const Type*, const Type*> tmp_compound_types;
	static std::map<std::pair<const Type*, std::vector<const Type*>>, const Function_type*> function_types;
	static std::map<std::pair<const Type*, std::vector<const Type*>>, const Type*> function_object_types;
	static std::map<std::pair<const Type*, std::vector<const Type*>>, const Type*> closure_types;
	static std::unordered_map<const Type*, const Type*> array_types;
	static std::unordered_map<const Type*, const Type*> const_array_types;
	static std::unordered_map<const Type*, const Type*> tmp_array_types;
	static std::unordered_map<const Type*, const Type*> set_types;
	static std::unordered_map<const Type*, const Type*> const_set_types;
	static std::unordered_map<const Type*, const Type*> tmp_set_types;
	static std::map<std::pair<const Type*, const Type*>, const Type*> map_types;
	static std::map<std::pair<const Type*, const Type*>, const Type*> const_map_types;
	static std::map<std::pair<const Type*, const Type*>, const Type*> tmp_map_types;
	static std::unordered_map<const Type*, const Type*> pointer_types;
	static std::unordered_map<const Type*, const Type*> temporary_types;
	static std::unordered_map<const Type*, const Type*> not_temporary_types;
	static std::unordered_map<const Type*, const Type*> const_types;
	static std::unordered_map<const Type*, const Type*> not_const_types;
	static std::unordered_map<std::string, const Type*> class_types;
	static std::unordered_map<std::string, const Type*> structure_types;
};

std::ostream& operator << (std::ostream&, const Type*);
std::ostream& operator << (std::ostream&, const std::vector<const Type*>&);
std::ostream& operator << (std::ostream&, const llvm::Type*);

}

#endif
