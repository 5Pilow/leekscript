#include "Variable.hpp"
#include "../../colors.h"
#include "../../type/Type.hpp"
#include "../value/Phi.hpp"

namespace ls {

Variable::Variable(std::string name, VarScope scope, const Type* type, int index, Value* value, FunctionVersion* function, Block* block, LSValue* lsvalue, Call call) : name(name), scope(scope), index(index), parent_index(0), value(value), function(function), block(block), type(type), lsvalue(lsvalue), call(call) {}

const Type* Variable::get_entry_type() const {
	if (type->is_mpz_ptr()) {
		return Type::mpz;
	}
	return type->not_temporary();
}

Compiler::value Variable::get_value(Compiler& c) const {
	if (!val.v) {
		std::cout << "no value for variable " << this << std::endl;
		assert(false);
	}
	// std::cout << "get_value " << this << std::endl;
	if (type->is_mpz_ptr()) {
		return val; // mpz values are passed by pointer so we don't load the value
	}
	return c.insn_load(val);
}

Compiler::value Variable::get_address(Compiler& c) const {
	if (!val.v) {
		std::cout << "no value for variable " << this << std::endl;
		assert(false);
	}
	return val;
}

void Variable::create_entry(Compiler& c) {
	// std::cout << "create_entry " << this << std::endl;
	auto t = get_entry_type();
	val = c.create_entry(name, t);
}
void Variable::create_addr_entry(Compiler& c, Compiler::value value) {
	// std::cout << "create_entry " << this << std::endl;
	auto t = get_entry_type()->pointer();
	addr_val = c.create_entry(name + "_addr", t);
	c.insn_store(addr_val, value);
}

void Variable::store_value(Compiler& c, Compiler::value value) {
	if (value.t->is_mpz_ptr()) {
		auto v = c.insn_load(value);
		c.insn_store(val, v);
		if (phi) {
			if (phi->variable1 == this) phi->value1 = v;
			if (phi->variable2 == this) phi->value2 = v;
		}
	} else {
		c.insn_store(val, value);
		if (phi) {
			if (phi->variable1 == this) phi->value1 = value;
			if (phi->variable2 == this) phi->value2 = value;
		}
	}
}

Variable* Variable::new_temporary(std::string name, const Type* type) {
	return new Variable(name, VarScope::LOCAL, type, 0, nullptr, nullptr, nullptr, nullptr);
}

const Type* Variable::get_type_for_variable_from_expression(const Type* expression_type) {
	if (expression_type->is_mpz() or expression_type->is_mpz_ptr()) {
		return Type::mpz_ptr;
	}
	return expression_type->not_temporary();
}

}

namespace ls {
	std::ostream& operator << (std::ostream& os, const Variable& variable) {
		os << BOLD << variable.name << END_COLOR;
		std::string versions;
		auto v = &variable;
		while (v and v->id != 0) {
			versions = C_GREY + std::string(".") + std::to_string(v->id) + END_COLOR + versions;
			v = v->root;
		}
		os << versions;
		// os << "." << (int) variable.scope;
		return os;
	}
	std::ostream& operator << (std::ostream& os, const Variable* variable) {
		os << *variable;
		// os << " " << (void*) variable;
		return os;
	}
}