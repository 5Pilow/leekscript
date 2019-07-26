#ifndef ENVIRONMENT_HPP
#define ENVIRONMENT_HPP

#include <vector>
#include <set>
#include <map>
#include <unordered_map>
#include "../type/Function_type.hpp"
#include "../standard/StandardLibrary.hpp"
#include "../compiler/Compiler.hpp"
#include "../vm/VM.hpp"

namespace ls {

class Type;

class Environment {
public:

	StandardLibrary std;
	Compiler compiler;
	VM vm;
    /*
     * Types
     */
    const Type* const void_;
	const Type* const never;
	const Type* const any;
	const Type* const tmp_any;
	const Type* const null;
	const Type* const const_any;
	const Type* const boolean;
	const Type* const const_boolean;
	const Type* const number;
	const Type* const const_number;
	const Type* const i8;
	const Type* const i8_ptr;
	const Type* const integer;
	const Type* const const_integer;
	const Type* const long_;
	const Type* const const_long;
	const Type* const mpz;
	const Type* const tmp_mpz;
	const Type* const const_mpz;
	const Type* const mpz_ptr;
	const Type* const tmp_mpz_ptr;
	const Type* const const_mpz_ptr;
	const Type* const tmp_string;
	const Type* const const_string;
	const Type* const real;
	const Type* const const_real;
	const Type* const string;
	const Type* const object;
	const Type* const tmp_object;
	const Type* const interval;
	const Type* const const_interval;
	const Type* const tmp_interval;

	std::vector<const Type*> placeholder_types;
    std::map<std::set<const Type*>, const Type*> compound_types;
	std::map<const Type*, const Type*> tmp_compound_types;
	std::map<std::pair<const Type*, std::vector<const Type*>>, const Function_type*> function_types;
	std::map<std::pair<const Type*, std::vector<const Type*>>, const Type*> function_object_types;
	std::map<std::pair<const Type*, std::vector<const Type*>>, const Type*> closure_types;
	std::unordered_map<const Type*, const Type*> array_types;
	std::unordered_map<const Type*, const Type*> const_array_types;
	std::unordered_map<const Type*, const Type*> tmp_array_types;
	std::unordered_map<const Type*, const Type*> set_types;
	std::unordered_map<const Type*, const Type*> const_set_types;
	std::unordered_map<const Type*, const Type*> tmp_set_types;
	std::map<std::pair<const Type*, const Type*>, const Type*> map_types;
	std::map<std::pair<const Type*, const Type*>, const Type*> const_map_types;
	std::map<std::pair<const Type*, const Type*>, const Type*> tmp_map_types;
	std::unordered_map<const Type*, const Type*> pointer_types;
	std::unordered_map<const Type*, const Type*> temporary_types;
	std::unordered_map<const Type*, const Type*> not_temporary_types;
	std::unordered_map<const Type*, const Type*> const_types;
	std::unordered_map<const Type*, const Type*> not_const_types;
	std::unordered_map<std::string, const Type*> class_types;
	std::unordered_map<std::string, const Type*> structure_types;

    Environment(bool legacy = false);

    const Type* array();
    const Type* array(const Type*);
	const Type* const_array(const Type*);
	const Type* tmp_array(const Type*);
	const Type* set(const Type*);
	const Type* const_set(const Type*);
	const Type* tmp_set(const Type*);
	const Type* map(const Type*, const Type*);
	const Type* tmp_map(const Type*, const Type*);
	const Type* const_map(const Type*, const Type*);
    const Type* fun();
	const Type* fun(const Type* return_type, std::vector<const Type*> arguments = {}, const Value* function = nullptr);
	const Type* fun_object(const Type* return_type, std::vector<const Type*> arguments = {}, const Value* function = nullptr);
	const Type* closure(const Type* return_type, std::vector<const Type*> arguments = {}, const Value* function = nullptr);
	const Type* structure(const std::string name, std::initializer_list<const Type*> types);
	const Type* clazz(const std::string name = "class?");
	const Type* const_class(const std::string name = "class?");
	const Type* template_(std::string name);
	const Type* compound(std::vector<const Type*> types);
	const Type* compound(std::initializer_list<const Type*> types);
	const Type* tmp_compound(std::initializer_list<const Type*> types);

	const Type* meta_add(const Type* t1, const Type* t2);
	const Type* meta_mul(const Type* t1, const Type* t2);
	const Type* meta_base_of(const Type* type, const Type* base);
	const Type* meta_not_temporary(const Type* type);

	const Type* generate_new_placeholder_type();
    void clear_placeholder_types();
};

}

#endif