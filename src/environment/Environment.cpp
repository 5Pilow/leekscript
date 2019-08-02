#include "Environment.hpp"
#include "../type/Void_type.hpp"
#include "../type/Any_type.hpp"
#include "../type/Array_type.hpp"
#include "../type/Set_type.hpp"
#include "../type/Function_type.hpp"
#include "../type/Function_object_type.hpp"
#include "../type/Class_type.hpp"
#include "../type/Object_type.hpp"
#include "../type/Class_type.hpp"
#include "../type/Interval_type.hpp"
#include "../type/String_type.hpp"
#include "../type/Number_type.hpp"
#include "../type/Long_type.hpp"
#include "../type/Bool_type.hpp"
#include "../type/Mpz_type.hpp"
#include "../type/Real_type.hpp"
#include "../type/Map_type.hpp"
#include "../type/Null_type.hpp"
#include "../type/Integer_type.hpp"
#include "../type/I8_type.hpp"
#include "../type/Placeholder_type.hpp"
#include "../type/Struct_type.hpp"
#include "../type/Pointer_type.hpp"
#include "../type/Template_type.hpp"
#include "../type/Never_type.hpp"
#include "../type/Compound_type.hpp"
#include "../type/Meta_add_type.hpp"
#include "../type/Meta_mul_type.hpp"
#include "../type/Meta_baseof_type.hpp"
#include "../type/Meta_element_type.hpp"
#include "../type/Meta_not_temporary_type.hpp"
#include "../analyzer/Program.hpp"
#include "../analyzer/syntaxic/SyntaxicAnalyzer.hpp"
#include "../analyzer/semantic/SemanticAnalyzer.hpp"
#include "../util/utf8.h"

namespace ls {

Environment::Environment() : Environment(false) {}

Environment::Environment(bool legacy) :
	#if COMPILER
	compiler(*this, &vm),
	vm(*this, std),
	#endif
	legacy(legacy),
    void_(new Void_type { *this }),
    boolean(new Bool_type { *this }),
    const_boolean(boolean->add_constant()),
    number(new Number_type { *this }),
    const_number(number->add_constant()),
    i8(new I8_type {*this }),
    i8_ptr(i8->pointer()),
    integer(new Integer_type { *this }),
    const_integer(integer->add_constant()),
    long_(new Long_type { *this }),
    const_long(long_->add_constant()),
    mpz(new Mpz_type { *this }),
    tmp_mpz(mpz->add_temporary()),
    const_mpz(mpz->add_constant()),
    mpz_ptr(mpz->pointer()),
    tmp_mpz_ptr(mpz_ptr->add_temporary()),
    const_mpz_ptr(mpz_ptr->add_constant()),
    never(new Never_type { *this }),
    any(new Any_type { *this }),
    tmp_any(any->add_temporary()),
    const_any(any->add_constant()),
    null(new Null_type { *this }),
    string(new String_type { *this }),
    tmp_string(string->add_temporary()),
    const_string(string->add_constant()),
    real(new Real_type { *this }),
    const_real(real->add_constant()),
    interval(new Interval_type { *this }),
    const_interval(interval->add_constant()),
    tmp_interval(interval->add_temporary()),
    object(new Object_type { *this }),
    tmp_object(object->add_temporary()),
	array(Type::array(void_)),
	set(Type::set(void_)),
	std(*this, legacy)
{}

void Environment::analyze(Program& program, bool format, bool debug) {
	auto resolver = new Resolver();
	SyntaxicAnalyzer syn { *this, resolver };
	SemanticAnalyzer sem { *this };
	program.analyze(syn, sem, format, debug);
}

#if COMPILER

void Environment::compile(Program& program, bool format, bool debug, bool ops, bool assembly, bool pseudo_code, bool optimized_ir, bool execute_ir, bool execute_bitcode) {
	vm.enable_operations = operation_limit > 0;
	program.compile(compiler, format, debug, assembly, pseudo_code, optimized_ir, execute_ir, execute_bitcode);
}

void Environment::execute(Program& program, bool format, bool debug, bool ops, bool assembly, bool pseudo_code, bool optimized_ir, bool execute_ir, bool execute_bitcode) {
	if (output) {
		vm.output = output;
	}
	vm.enable_operations = operation_limit > 0;
	if (operation_limit != -1) {
		vm.operation_limit = operation_limit;
	}
	vm.execute(program, format, debug, ops, assembly, pseudo_code, optimized_ir, execute_ir, execute_bitcode);
}

#endif

const Type* Environment::generate_new_placeholder_type() {
	uint32_t character = 0x03B1 + Type::placeholder_counter;
	char buff[5];
	u8_toutf8(buff, 5, &character, 1);
	auto type = new Placeholder_type(*this, std::string { buff });
	Type::placeholder_counter++;
	Environment::placeholder_types.push_back(type);
	return type;
}
void Environment::clear_placeholder_types() {
	placeholder_types.clear();
}
const Type* Environment::template_(std::string name) {
	return new Template_type(*this, name);
}

const Type* Environment::clazz(const std::string name) {
	auto i = class_types.find(name);
	if (i != class_types.end()) return i->second;
	auto type = new Class_type(*this, name);
	class_types.insert({ name, type });
	return type;
}
const Type* Environment::const_class(const std::string name) {
	return clazz(name)->add_constant();
}

}