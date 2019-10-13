#include "Variable.hpp"
#include "../../colors.h"
#include "../../type/Type.hpp"
#include "../value/Phi.hpp"
#include "../../environment/Environment.hpp"

namespace ls {

Variable::Variable(std::string name, VarScope scope, const Type* type, int index, Value* value, FunctionVersion* function, Block* block, Section* section, Class* clazz, Call call, bool global) : name(name), scope(scope), index(index), parent_index(0), value(value), function(function), block(block), section(section), type(type), clazz(clazz), call(call), global(global)
#if COMPILER
, entry(type->env), addr_val(type->env), val(type->env)
#endif
{}

#if COMPILER
Variable::Variable(std::string name, VarScope scope, const Type* type, int index, Value* value, FunctionVersion* function, Block* block, Section* section, Class* clazz, LSClass* lsclass, Call call) : name(name), scope(scope), index(index), parent_index(0), value(value), function(function), block(block), section(section), type(type), clazz(clazz), lsclass(lsclass), call(call), entry(type->env), addr_val(type->env), val(type->env) {}
#endif

const Type* Variable::get_entry_type(Environment& env) const {
	if (type->is_mpz_ptr()) {
		return env.mpz;
	}
	return type->not_temporary();
}

#if COMPILER
Compiler::value Variable::get_value(Compiler& c) const {
	if (val.v) {
		return val;
	}
	if (!entry.v) {
		std::cout << "no value for variable " << this << std::endl;
		assert(false);
	}
	// std::cout << "get_value " << this << std::endl;
	if (type->is_mpz_ptr()) {
		return entry; // mpz values are passed by pointer so we don't load the value
	}
	return c.insn_load(entry);
}

Compiler::value Variable::get_address(Compiler& c) const {
	if (!entry.v) {
		std::cout << "no value for variable " << this << std::endl;
		assert(false);
	}
	return entry;
}

void Variable::create_entry(Compiler& c) {
	// std::cout << "create_entry " << this << std::endl;
	auto t = get_entry_type(c.env);
	entry = c.create_entry(name, t);
}
void Variable::create_addr_entry(Compiler& c, Compiler::value value) {
	// std::cout << "create_addr_entry " << this << std::endl;
	auto t = get_entry_type(c.env)->pointer();
	addr_val = c.create_entry(name + "_addr", t);
	c.insn_store(addr_val, value);
}

void Variable::store_value(Compiler& c, Compiler::value value) {
	if (value.t->is_mpz_ptr()) {
		auto v = c.insn_load(value);
		c.insn_store(entry, v);
		for (const auto& phi : phis) {
			if (phi->variable1 == this) phi->value1 = v;
			if (phi->variable2 == this) phi->value2 = v;
		}
	} else {
		c.insn_store(entry, value);
		for (const auto& phi : phis) {
			if (phi->variable1 == this) phi->value1 = value;
			if (phi->variable2 == this) phi->value2 = value;
		}
	}
}

void Variable::delete_value(Compiler& c) {
	if (type->is_mpz_ptr()) {
		if (entry.v) {
			c.insn_delete_mpz(entry);
		}
	} else if (type->must_manage_memory()) {
		if (entry.v) {
			// std::cout << "Delete variable " << name << " entry" << std::endl;
			c.insn_delete(c.insn_load(entry));
		} else if (val.v) {
			c.insn_delete(val);
		}
	}
}
#endif

Variable* Variable::new_temporary(std::string name, const Type* type) {
	return new Variable(name, VarScope::LOCAL, type, 0, nullptr, nullptr, nullptr, nullptr, nullptr);
}

const Type* Variable::get_type_for_variable_from_expression(Environment& env, const Value* expression) {
	if (not expression) {
		return env.never;
	}
	if (expression->type->is_mpz() or expression->type->is_mpz_ptr()) {
		return env.mpz_ptr;
	}
	return expression->type->not_temporary();
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