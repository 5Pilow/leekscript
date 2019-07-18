#include "SetSTD.hpp"
#include "../value/LSSet.hpp"
#include "../../type/Type.hpp"

namespace ls {
	
const std::_Rb_tree_node_base* iterator_end(LSSet<int>* set) {
	return set->end()._M_node;
}
LSSet<int>::iterator iterator_inc(LSSet<int>::iterator it) {
	it++;
	return it;
}

SetSTD::SetSTD(VM* vm) : Module(vm, "Set") {

	#if COMPILER
	LSSet<LSValue*>::clazz = lsclass.get();
	LSSet<int>::clazz = lsclass.get();
	LSSet<double>::clazz = lsclass.get();
	#endif

	/*
	 * Constructor
	 */
	constructor_({
		{Type::tmp_set(Type::any), {}, (void*) &LSSet<LSValue*>::constructor},
		{Type::tmp_set(Type::real), {}, (void*) &LSSet<double>::constructor},
		{Type::tmp_set(Type::integer), {}, (void*) &LSSet<int>::constructor},
	});

	/*
	 * Operators
	 */
	operator_("in", {
		{Type::const_set(), Type::any, Type::boolean, (void*) &LSSet<LSValue*>::in_v},
		{Type::const_set(), Type::integer, Type::boolean, ADDR(in_any)},
		{Type::const_set(Type::real), Type::real, Type::boolean, (void*) &LSSet<double>::in_v},
		{Type::const_set(Type::integer), Type::integer, Type::boolean, (void*) &LSSet<int>::in_v}
	});

	auto pqT = Type::template_("T");
	auto pqE = Type::template_("E");
	template_(pqT, pqE).
	operator_("+=", {
		{Type::set(pqT), pqE, Type::set(Type::meta_mul(pqT, pqE)), ADDR(set_add_eq), 0, { new ConvertMutator() }, true},
	});

	/*
	 * Methods
	 */
	method("size", {
		{Type::integer, {Type::const_set()}, (void*) &LSSet<LSValue*>::ls_size},
		{Type::integer, {Type::const_set(Type::real)}, (void*) &LSSet<double>::ls_size},
		{Type::integer, {Type::const_set(Type::integer)}, (void*) &LSSet<int>::ls_size},
	});
	method("insert", {
		{Type::boolean, {Type::set(Type::any), Type::any}, (void*) &LSSet<LSValue*>::ls_insert_ptr},
		{Type::boolean, {Type::set(Type::any), Type::any}, ADDR(insert_any)},
		{Type::boolean, {Type::set(Type::real), Type::real}, ADDR(insert_real)},
		{Type::boolean, {Type::set(Type::integer), Type::integer}, ADDR(insert_int)},
	});
	method("clear", {
		{Type::set(), {Type::set(Type::any)}, (void*) &LSSet<LSValue*>::ls_clear},
		{Type::set(Type::real), {Type::set(Type::real)}, (void*) &LSSet<double>::ls_clear},
		{Type::set(Type::integer), {Type::set(Type::integer)}, (void*) &LSSet<int>::ls_clear},
	});
	method("erase", {
		{Type::boolean, {Type::set(), Type::any}, (void*) &LSSet<LSValue*>::ls_erase},
		{Type::boolean, {Type::set(Type::real), Type::real}, (void*) &LSSet<double>::ls_erase},
		{Type::boolean, {Type::set(Type::integer), Type::integer}, (void*) &LSSet<int>::ls_erase},
	});
	method("contains", {
		{Type::boolean, {Type::const_set(), Type::any}, (void*) &LSSet<LSValue*>::ls_contains},
		{Type::boolean, {Type::const_set(Type::real), Type::real}, (void*) &LSSet<double>::ls_contains},
		{Type::boolean, {Type::const_set(Type::integer), Type::integer}, (void*) &LSSet<int>::ls_contains},
	});

	/** Internal **/
	method("vinsert", {
		{Type::void_, {Type::const_set(Type::any), Type::any}, (void*) &LSSet<LSValue*>::vinsert},
		{Type::void_, {Type::const_set(Type::real), Type::real}, (void*) &LSSet<double>::vinsert},
		{Type::void_, {Type::const_set(Type::integer), Type::integer}, (void*) &LSSet<int>::vinsert},
	});
	method("iterator_end", {
		{Type::set()->iterator(), {Type::set()}, (void*) iterator_end}
	});
	method("iterator_inc", {
		{Type::set()->iterator(), {Type::set()->iterator()}, (void*) iterator_inc}
	});
	method("insert_fun", {
		{Type::boolean, {Type::const_set(Type::any), Type::any}, (void*) &LSSet<LSValue*>::ls_insert},
		{Type::boolean, {Type::const_set(Type::real), Type::real}, (void*) &LSSet<double>::ls_insert},
		{Type::boolean, {Type::const_set(Type::integer), Type::integer}, (void*) &LSSet<int>::ls_insert},
	});
	method("int_to_any", {
		{Type::set(Type::any), {Type::set(Type::integer)}, (void*) &LSSet<int>::to_any_set}
	});
	method("real_to_any", {
		{Type::set(Type::any), {Type::set(Type::real)}, (void*) &LSSet<double>::to_any_set}
	});
}

#if COMPILER

Compiler::value SetSTD::in_any(Compiler& c, std::vector<Compiler::value> args, int) {
	return c.insn_call(Type::any, {args[0], c.insn_to_any(args[1])}, "Value.operatorin");
}

Compiler::value SetSTD::set_add_eq(Compiler& c, std::vector<Compiler::value> args, int) {
	return c.insn_call(Type::any, {args[0], c.insn_to_any(args[1])}, "Value.operator+=");
}

Compiler::value SetSTD::insert_any(Compiler& c, std::vector<Compiler::value> args, int) {
	return c.insn_call(Type::boolean, {args[0], c.insn_to_any(args[1])}, "Set.insert_fun");
}
Compiler::value SetSTD::insert_real(Compiler& c, std::vector<Compiler::value> args, int) {
	return c.insn_call(Type::boolean, {args[0], c.to_real(args[1])}, "Set.insert_fun.1");
}
Compiler::value SetSTD::insert_int(Compiler& c, std::vector<Compiler::value> args, int) {
	return c.insn_call(Type::boolean, {args[0], c.to_int(args[1])}, "Set.insert_fun.2");
}

#endif

}
