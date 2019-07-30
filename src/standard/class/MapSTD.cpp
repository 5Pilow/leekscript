#include "MapSTD.hpp"
#include "../../type/Type.hpp"
#include "../../analyzer/semantic/Variable.hpp"
#include "../../environment/Environment.hpp"
#if COMPILER
#include "../../vm/value/LSMap.hpp"
#endif

namespace ls {

#if COMPILER
int map_size(const LSMap<LSValue*,LSValue*>* map) {
	int r = map->size();
	if (map->refs == 0) delete map;
	return r;
}
void* iterator_end(LSMap<int, int>* map) {
	return map->end()._M_node;
}
LSMap<int, int>::iterator iterator_inc(LSMap<int, int>::iterator it) {
	it++;
	return it;
}
LSMap<int, int>::iterator iterator_dec(LSMap<int, int>::iterator it) {
	it--;
	return it;
}
void* iterator_rkey(std::map<void*, void*>::iterator it) {
	return std::map<void*, void*>::reverse_iterator(it)->first;
}
int iterator_rget_ii(std::map<int, int>::iterator it) {
	return std::map<int, int>::reverse_iterator(it)->second;
}
int iterator_rget_vi(std::map<void*, int>::iterator it) {
	return std::map<void*, int>::reverse_iterator(it)->second;
}
double iterator_rget_ir(std::map<int, double>::iterator it) {
	return std::map<int, double>::reverse_iterator(it)->second;
}
void* iterator_rget_vv(std::map<void*, void*>::iterator it) {
	return std::map<void*, void*>::reverse_iterator(it)->second;
}
std::map<int, int>::iterator end(LSMap<int, int>* map) {
	return map->end();
}
#endif

MapSTD::MapSTD(Environment& env) : Module(env, "Map") {

	#if COMPILER
	env.map_class = std::make_unique<LSClass>(clazz.get());
	lsclass = env.map_class.get();
	#endif

	/*
	 * Constructor
	 */
	constructor_({
		{Type::tmp_map(env.any, env.any), {}, ADDR(((void*) &LSMap<LSValue*, LSValue*>::constructor))},
		{Type::tmp_map(env.any, env.real), {}, ADDR(((void*) &LSMap<LSValue*, double>::constructor))},
		{Type::tmp_map(env.any, env.integer), {}, ADDR(((void*) &LSMap<LSValue*, int>::constructor))},
		{Type::tmp_map(env.real, env.any), {}, ADDR(((void*) &LSMap<double, LSValue*>::constructor))},
		{Type::tmp_map(env.real, env.real), {}, ADDR(((void*) &LSMap<double, double>::constructor))},
		{Type::tmp_map(env.real, env.integer), {}, ADDR(((void*) &LSMap<double, int>::constructor))},
		{Type::tmp_map(env.integer, env.any), {}, ADDR(((void*) &LSMap<int, LSValue*>::constructor))},
		{Type::tmp_map(env.integer, env.real), {}, ADDR(((void*) &LSMap<int, double>::constructor))},
		{Type::tmp_map(env.integer, env.integer), {}, ADDR(((void*) &LSMap<int, int>::constructor))},
	});

	operator_("in", {
		{Type::const_map(env.any, env.any), env.any, env.boolean, ADDR(((void*) &LSMap<LSValue*, LSValue*>::in))},
		{Type::const_map(env.any, env.real), env.any, env.boolean, ADDR(((void*) &LSMap<LSValue*, double>::in))},
		{Type::const_map(env.any, env.integer), env.any, env.boolean, ADDR(((void*) &LSMap<LSValue*, int>::in))},
		{Type::const_map(env.real, env.any), env.real, env.boolean, ADDR(((void*) &LSMap<double, LSValue*>::in))},
		{Type::const_map(env.real, env.real), env.real, env.boolean, ADDR(((void*) &LSMap<double, double>::in))},
		{Type::const_map(env.real, env.integer), env.real, env.boolean, ADDR(((void*) &LSMap<double, int>::in))},
		{Type::const_map(env.integer, env.any), env.integer, env.boolean, ADDR(((void*) &LSMap<int, LSValue*>::in))},
		{Type::const_map(env.integer, env.real), env.integer, env.boolean, ADDR(((void*) &LSMap<int, double>::in))},
		{Type::const_map(env.integer, env.integer), env.long_, env.boolean, ADDR(((void*) &LSMap<int, int>::in))},
	});

	method("size", {
		{env.integer, {Type::const_map(env.any, env.any)}, ADDR((void*) map_size)},
		{env.integer, {Type::const_map(env.any, env.real)}, ADDR((void*) map_size)},
		{env.integer, {Type::const_map(env.any, env.integer)}, ADDR((void*) map_size)},
		{env.integer, {Type::const_map(env.integer, env.any)}, ADDR((void*) map_size)},
		{env.integer, {Type::const_map(env.integer, env.real)}, ADDR((void*) map_size)},
		{env.integer, {Type::const_map(env.integer, env.integer)}, ADDR((void*) map_size)},
    });

	method("values", {
		{Type::array(env.any), {Type::const_map(env.any, env.any)}, ADDR(((void*) &LSMap<LSValue*, LSValue*>::values))},
		{Type::array(env.real), {Type::const_map(env.any, env.real)}, ADDR(((void*) &LSMap<LSValue*, double>::values))},
		{Type::array(env.integer), {Type::const_map(env.any, env.integer)}, ADDR(((void*) &LSMap<LSValue*, int>::values))},
		{Type::array(env.any), {Type::const_map(env.real, env.any)}, ADDR(((void*) &LSMap<double, LSValue*>::values))},
		{Type::array(env.real), {Type::const_map(env.real, env.real)}, ADDR(((void*) &LSMap<double, double>::values))},
		{Type::array(env.integer), {Type::const_map(env.real, env.integer)}, ADDR(((void*) &LSMap<double, int>::values))},
		{Type::array(env.any), {Type::const_map(env.integer, env.any)}, ADDR(((void*) &LSMap<int, LSValue*>::values))},
		{Type::array(env.real), {Type::const_map(env.integer, env.real)}, ADDR(((void*) &LSMap<int, double>::values))},
		{Type::array(env.integer), {Type::const_map(env.integer, env.integer)}, ADDR(((void*) &LSMap<int, int>::values))}
	});

	method("insert", {
		{env.boolean, {Type::map(env.any, env.any), env.any, env.any}, ADDR(((void*) &LSMap<LSValue*, LSValue*>::ls_insert))},
		{env.boolean, {Type::map(env.any, env.real), env.any, env.real}, ADDR(((void*) &LSMap<LSValue*, double>::ls_insert))},
		{env.boolean, {Type::map(env.any, env.integer), env.any, env.integer}, ADDR(((void*) &LSMap<LSValue*, int>::ls_insert))},
		{env.boolean, {Type::map(env.integer, env.any), env.integer, env.any}, ADDR(((void*) &LSMap<int, LSValue*>::ls_insert))},
		{env.boolean, {Type::map(env.integer, env.real), env.integer, env.real}, ADDR(((void*) &LSMap<int, double>::ls_insert))},
		{env.boolean, {Type::map(env.integer, env.integer), env.integer, env.integer}, ADDR(((void*) &LSMap<int, int>::ls_insert))},
    });

	method("clear", {
		{Type::map(env.any, env.any), {Type::map(env.void_, env.void_)}, ADDR(((void*) &LSMap<LSValue*,LSValue*>::ls_clear))},
		{Type::map(env.any, env.real), {Type::map(env.any, env.real)}, ADDR(((void*) &LSMap<LSValue*,double>::ls_clear))},
		{Type::map(env.any, env.integer), {Type::map(env.any, env.integer)}, ADDR(((void*) &LSMap<LSValue*,int>::ls_clear))},
		{Type::map(env.integer, env.any), {Type::map(env.integer, env.any)}, ADDR(((void*) &LSMap<int,LSValue*>::ls_clear))},
		{Type::map(env.integer, env.real), {Type::map(env.integer, env.real)}, ADDR(((void*) &LSMap<int,double>::ls_clear))},
		{Type::map(env.integer, env.integer), {Type::map(env.integer, env.integer)}, ADDR(((void*) &LSMap<int,int>::ls_clear))},
	});

	method("erase", {
		{env.boolean, {Type::map(env.any, env.any), env.any}, ADDR(((void*) &LSMap<LSValue*,LSValue*>::ls_erase))},
		{env.boolean, {Type::map(env.any, env.real), env.any}, ADDR(((void*) &LSMap<LSValue*,double>::ls_erase))},
		{env.boolean, {Type::map(env.any, env.integer), env.any}, ADDR(((void*) &LSMap<LSValue*,int>::ls_erase))},
		{env.boolean, {Type::map(env.integer, env.any), env.integer}, ADDR(((void*) &LSMap<int,LSValue*>::ls_erase))},
		{env.boolean, {Type::map(env.integer, env.real), env.integer}, ADDR(((void*) &LSMap<int,double>::ls_erase))},
		{env.boolean, {Type::map(env.integer, env.integer), env.integer}, ADDR(((void*) &LSMap<int,int>::ls_erase))},
	});

	auto lK = env.template_("K");
	auto lV = env.template_("V");
	template_(lK, lV).
	method("look", {
		{lV, {Type::const_map(lK, lV), lK, lV}, ADDR(look)},
	});

	method("min", {
		{env.any, {Type::const_map(env.any, env.any)}, ADDR(((void*) &LSMap<LSValue*,LSValue*>::ls_min)), THROWS},
		{env.real, {Type::const_map(env.any, env.real)}, ADDR(((void*) &LSMap<LSValue*,double>::ls_min)), THROWS},
		{env.integer, {Type::const_map(env.any, env.integer)}, ADDR(((void*) &LSMap<LSValue*,int>::ls_min)), THROWS},
		{env.any, {Type::const_map(env.integer, env.any)}, ADDR(((void*) &LSMap<int,LSValue*>::ls_min)), THROWS},
		{env.real, {Type::const_map(env.integer, env.real)}, ADDR(((void*) &LSMap<int,double>::ls_min)), THROWS},
		{env.integer, {Type::const_map(env.integer, env.integer)}, ADDR(((void*) &LSMap<int,int>::ls_min)), THROWS},
	});

	method("minKey", {
		{env.any, {Type::const_map(env.any, env.any)}, ADDR(((void*) &LSMap<LSValue*,LSValue*>::ls_minKey)), THROWS},
		{env.any, {Type::const_map(env.any, env.real)}, ADDR(((void*) &LSMap<LSValue*,double>::ls_minKey)), THROWS},
		{env.any, {Type::const_map(env.any, env.integer)}, ADDR(((void*) &LSMap<LSValue*,int>::ls_minKey)), THROWS},
		{env.integer, {Type::const_map(env.integer, env.any)}, ADDR(((void*) &LSMap<int,LSValue*>::ls_minKey)), THROWS},
		{env.integer, {Type::const_map(env.integer, env.real)}, ADDR(((void*) &LSMap<int,double>::ls_minKey)), THROWS},
		{env.integer, {Type::const_map(env.integer, env.integer)}, ADDR(((void*) &LSMap<int,int>::ls_minKey)), THROWS},
	});

	method("max", {
		{env.any, {Type::const_map(env.any, env.any)}, ADDR(((void*) &LSMap<LSValue*,LSValue*>::ls_max)), THROWS},
		{env.real, {Type::const_map(env.any, env.real)}, ADDR(((void*) &LSMap<LSValue*,double>::ls_max)), THROWS},
		{env.integer, {Type::const_map(env.any, env.integer)}, ADDR(((void*) &LSMap<LSValue*,int>::ls_max)), THROWS},
		{env.any, {Type::const_map(env.integer, env.any)}, ADDR(((void*) &LSMap<int,LSValue*>::ls_max)), THROWS},
		{env.real, {Type::const_map(env.integer, env.real)}, ADDR(((void*) &LSMap<int,double>::ls_max)), THROWS},
		{env.integer, {Type::const_map(env.integer, env.integer)}, ADDR(((void*) &LSMap<int,int>::ls_max)), THROWS},
	});

	method("maxKey", {
		{env.any, {Type::const_map(env.any, env.any)}, ADDR(((void*) &LSMap<LSValue*,LSValue*>::ls_maxKey)), THROWS},
		{env.any, {Type::const_map(env.any, env.real)}, ADDR(((void*) &LSMap<LSValue*,double>::ls_maxKey)), THROWS},
		{env.any, {Type::const_map(env.any, env.integer)}, ADDR(((void*) &LSMap<LSValue*,int>::ls_maxKey)), THROWS},
		{env.integer, {Type::const_map(env.integer, env.any)}, ADDR(((void*) &LSMap<int,LSValue*>::ls_maxKey)), THROWS},
		{env.integer, {Type::const_map(env.integer, env.real)}, ADDR(((void*) &LSMap<int,double>::ls_maxKey)), THROWS},
		{env.integer, {Type::const_map(env.integer, env.integer)}, ADDR(((void*) &LSMap<int,int>::ls_maxKey)), THROWS},
	});

	auto iter_ptr = ADDR((&LSMap<LSValue*, LSValue*>::ls_iter<LSFunction*>));
	auto iK = env.template_("K");
	auto iV = env.template_("V");
	template_(iK, iV).
	method("iter", {
		{env.void_, {Type::const_map(env.any, env.any), Type::fun(env.void_, {env.any, env.any})}, (void*) iter_ptr, THROWS},
		{env.void_, {Type::const_map(iK, iV), Type::fun(env.void_, {iK, iV})}, ADDR(iter), THROWS},
	});

	auto flT = env.template_("T");
	auto flK = env.template_("K");
	auto flI = env.template_("I");
	auto flR = env.template_("R");
	template_(flT, flK, flI, flR).
	method("foldLeft", {
		{flR, {Type::const_map(flK, flT), Type::fun(flR, {Type::meta_add(flI, flR), flK, flT}), flI}, ADDR(fold_left)},
	});

	auto frT = env.template_("T");
	auto frK = env.template_("K");
	auto frI = env.template_("I");
	auto frR = env.template_("R");
	template_(frT, frK, frI, frR).
	method("foldRight", {
		{frR, {Type::const_map(frK, frT), Type::fun(frR, {frK, frT, Type::meta_add(frI, frR)}), frI}, ADDR(fold_right)},
	});

	/** Internal **/
	method("at", {
		{env.integer, {Type::map(env.integer, env.integer), env.integer}, ADDR(((void*) &LSMap<int, int>::at))},
		{env.real, {Type::map(env.integer, env.real), env.integer}, ADDR(((void*) &LSMap<int, double>::at))},
		{env.any, {Type::map(env.integer, env.any), env.integer}, ADDR(((void*) &LSMap<int, LSValue*>::at))},
		{env.integer, {Type::map(env.real, env.integer), env.real}, ADDR(((void*) &LSMap<double, int>::at))},
		{env.real, {Type::map(env.real, env.real), env.real}, ADDR(((void*) &LSMap<double, double>::at))},
		{env.any, {Type::map(env.real, env.any), env.real}, ADDR(((void*) &LSMap<double, LSValue*>::at))},
		{env.integer, {Type::map(env.any, env.integer), env.any}, ADDR(((void*) &LSMap<LSValue*, int>::at))},
		{env.real, {Type::map(env.any, env.real), env.any}, ADDR(((void*) &LSMap<LSValue*, double>::at))},
		{env.any, {Type::map(env.any, env.any), env.any}, ADDR(((void*) &LSMap<LSValue*, LSValue*>::at))},
	});
	method("insert_fun", {
		{env.void_, {Type::map(env.any, env.any), env.any, env.any}, ADDR(((void*) &LSMap<LSValue*, LSValue*>::ls_emplace))},
		{env.void_, {Type::map(env.any, env.real), env.any, env.real}, ADDR(((void*) &LSMap<LSValue*, double>::ls_emplace))},
		{env.void_, {Type::map(env.any, env.integer), env.any, env.integer}, ADDR(((void*) &LSMap<LSValue*, int>::ls_emplace))},
		{env.void_, {Type::map(env.real, env.any), env.real, env.any}, ADDR(((void*) &LSMap<double, LSValue*>::ls_emplace))},
		{env.void_, {Type::map(env.real, env.real), env.real, env.real}, ADDR(((void*) &LSMap<double, double>::ls_emplace))},
		{env.void_, {Type::map(env.real, env.integer), env.real, env.integer}, ADDR(((void*) &LSMap<double, int>::ls_emplace))},
		{env.void_, {Type::map(env.integer, env.any), env.integer, env.any}, ADDR(((void*) &LSMap<int, LSValue*>::ls_emplace))},
		{env.void_, {Type::map(env.integer, env.real), env.integer, env.real}, ADDR(((void*) &LSMap<int, double>::ls_emplace))},
		{env.void_, {Type::map(env.integer, env.integer), env.integer, env.integer}, ADDR(((void*) &LSMap<int, int>::ls_emplace))},
	});
	method("atL", {
		{env.void_, {Type::map(env.any, env.any), env.any, env.any}, ADDR(((void*) &LSMap<LSValue*, LSValue*>::atL_base))},
		{env.void_, {Type::map(env.any, env.real), env.any, env.real}, ADDR(((void*) &LSMap<LSValue*, double>::atL_base))},
		{env.void_, {Type::map(env.any, env.integer), env.any, env.integer}, ADDR(((void*) &LSMap<LSValue*, int>::atL_base))},
		{env.void_, {Type::map(env.real, env.any), env.real, env.any}, ADDR(((void*) &LSMap<double, LSValue*>::atL_base))},
		{env.void_, {Type::map(env.real, env.real), env.real, env.real}, ADDR(((void*) &LSMap<double, double>::atL_base))},
		{env.void_, {Type::map(env.real, env.integer), env.real, env.integer}, ADDR(((void*) &LSMap<double, int>::atL_base))},
		{env.void_, {Type::map(env.integer, env.any), env.integer, env.any}, ADDR(((void*) &LSMap<int, LSValue*>::atL_base))},
		{env.void_, {Type::map(env.integer, env.real), env.integer, env.real}, ADDR(((void*) &LSMap<int, double>::atL_base))},
		{env.void_, {Type::map(env.integer, env.integer), env.integer, env.integer}, ADDR(((void*) &LSMap<int, int>::atL_base))},
	});
	// std::map<int, int>::iterator (LSMap<int, int>::*mapend)() = &LSMap<int, int>::end;
	method("end", {
		{Type::map(env.void_, env.void_)->iterator(), {Type::map(env.void_, env.void_)}, ADDR((void*) end)}
	});
	method("iterator_end", {
		{Type::map(env.void_, env.void_)->iterator(), {Type::map(env.void_, env.void_)}, ADDR((void*) iterator_end)}
	});
	method("iterator_inc", {
		{Type::map(env.void_, env.void_)->iterator(), {Type::map(env.void_, env.void_)->iterator()}, ADDR((void*) iterator_inc)}
	});
	method("iterator_dec", {
		{Type::map(env.void_, env.void_)->iterator(), {Type::map(env.void_, env.void_)->iterator()}, ADDR((void*) iterator_dec)}
	});
	method("iterator_rkey", {
		{env.i8_ptr, {Type::map(env.void_, env.void_)->iterator()}, ADDR((void*) iterator_rkey)}
	});
	method("iterator_rget", {
		{env.integer, {Type::map(env.void_, env.void_)->iterator()}, ADDR((void*) iterator_rget_ii)},
		{env.integer, {Type::map(env.void_, env.void_)->iterator()}, ADDR((void*) iterator_rget_vi)},
		{env.real, {Type::map(env.void_, env.void_)->iterator()}, ADDR((void*) iterator_rget_ir)},
		{env.any, {Type::map(env.void_, env.void_)->iterator()}, ADDR((void*) iterator_rget_vv)},
	});
	method("look_fun", {
		{env.any, {Type::const_map(env.any, env.any), env.any, env.any}, ADDR(((void*) &LSMap<LSValue*, LSValue*>::ls_look))},
		{env.real, {Type::const_map(env.any, env.real), env.any, env.real}, ADDR(((void*) &LSMap<LSValue*, double>::ls_look))},
		{env.integer, {Type::const_map(env.any, env.integer), env.any, env.integer}, ADDR(((void*) &LSMap<LSValue*, int>::ls_look))},
		{env.any, {Type::const_map(env.integer, env.any), env.integer, env.any}, ADDR(((void*) &LSMap<int, LSValue*>::ls_look))},
		{env.real, {Type::const_map(env.integer, env.real), env.integer, env.real}, ADDR(((void*) &LSMap<int, double>::ls_look))},
		{env.integer, {Type::const_map(env.integer, env.integer), env.integer, env.integer}, ADDR(((void*) &LSMap<int, int>::ls_look))},
	});
}

#if COMPILER

Compiler::value MapSTD::look(Compiler& c, std::vector<Compiler::value> args, int) {
	auto map = args[0];
	auto f = [&]() {
		if (map.t->key()->is_integer()) {
			if (map.t->element()->is_integer()) return "Map.look_fun.5";
			if (map.t->element()->is_real()) return "Map.look_fun.4";
			return "Map.look_fun.3";
		}
		if (map.t->element()->is_integer()) return "Map.look_fun.2";
		if (map.t->element()->is_real()) return "Map.look_fun.1";
		return "Map.look_fun";
	}();
	return c.insn_call(map.t->element(), {args[0], args[1], args[2]}, f);
}

Compiler::value MapSTD::fold_left(Compiler& c, std::vector<Compiler::value> args, int) {
	auto function = args[1];
	auto init_type = function.t->argument(0);
	auto result = Variable::new_temporary("r", init_type);
	result->create_entry(c);
	c.insn_store(result->val, c.insn_convert(c.insn_move(args[2]), init_type));
	c.add_temporary_variable(result);
	auto v = Variable::new_temporary("v", args[0].t->element());
	auto k = Variable::new_temporary("k", args[0].t->key());
	v->create_entry(c);
	k->create_entry(c);
	c.add_temporary_variable(v);
	c.add_temporary_variable(k);
	c.insn_foreach(args[0], c.env.void_, v, k, [&](Compiler::value v, Compiler::value k) -> Compiler::value {
		c.insn_store(result->val, c.insn_call(function, {c.insn_load(result->val), k, v}));
		return { c.env };
	});
	return c.insn_load(result->val);
}

Compiler::value MapSTD::fold_right(Compiler& c, std::vector<Compiler::value> args, int) {
	auto function = args[1];
	auto init_type = function.t->argument(2);
	auto result = Variable::new_temporary("r", init_type);
	result->create_entry(c);
	c.insn_store(result->val, c.insn_convert(c.insn_move(args[2]), init_type));
	c.add_temporary_variable(result);
	auto v = Variable::new_temporary("v", args[0].t->element());
	auto k = Variable::new_temporary("k", args[0].t->key());
	v->create_entry(c);
	k->create_entry(c);
	c.add_temporary_variable(v);
	c.add_temporary_variable(k);
	c.insn_foreach(args[0], c.env.void_, v, k, [&](Compiler::value v, Compiler::value k) -> Compiler::value {
		c.insn_store(result->val, c.insn_call(function, {k, v, c.insn_load(result->val)}));
		return { c.env };
	}, true);
	return c.insn_load(result->val);
}

Compiler::value MapSTD::iter(Compiler& c, std::vector<Compiler::value> args, int) {
	auto function = args[1];
	auto v = Variable::new_temporary("v", args[0].t->element());
	auto k = Variable::new_temporary("k", args[0].t->key());
	v->create_entry(c);
	k->create_entry(c);
	c.add_temporary_variable(v);
	c.add_temporary_variable(k);
	c.insn_foreach(args[0], c.env.void_, v, k, [&](Compiler::value v, Compiler::value k) -> Compiler::value {
		return c.insn_call(function, {k, v});
	});
	return { c.env };
}

#endif

}
