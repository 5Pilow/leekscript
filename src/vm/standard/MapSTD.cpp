#include "MapSTD.hpp"
#include "../value/LSMap.hpp"
#include "../../type/Type.hpp"
#include "../../analyzer/semantic/Variable.hpp"

namespace ls {

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

MapSTD::MapSTD(VM* vm) : Module(vm, "Map") {

	LSMap<LSValue*, LSValue*>::clazz = clazz.get();
	LSMap<LSValue*, int>::clazz = clazz.get();
	LSMap<LSValue*, double>::clazz = clazz.get();
	LSMap<int, LSValue*>::clazz = clazz.get();
	LSMap<int, int>::clazz = clazz.get();
	LSMap<int, double>::clazz = clazz.get();
	LSMap<double, LSValue*>::clazz = clazz.get();
	LSMap<double, int>::clazz = clazz.get();
	LSMap<double, double>::clazz = clazz.get();

	/*
	 * Constructor
	 */
	constructor_({
		{Type::tmp_map(Type::any, Type::any), {}, (void*) &LSMap<LSValue*, LSValue*>::constructor},
		{Type::tmp_map(Type::any, Type::real), {}, (void*) &LSMap<LSValue*, double>::constructor},
		{Type::tmp_map(Type::any, Type::integer), {}, (void*) &LSMap<LSValue*, int>::constructor},
		{Type::tmp_map(Type::real, Type::any), {}, (void*) &LSMap<double, LSValue*>::constructor},
		{Type::tmp_map(Type::real, Type::real), {}, (void*) &LSMap<double, double>::constructor},
		{Type::tmp_map(Type::real, Type::integer), {}, (void*) &LSMap<double, int>::constructor},
		{Type::tmp_map(Type::integer, Type::any), {}, (void*) &LSMap<int, LSValue*>::constructor},
		{Type::tmp_map(Type::integer, Type::real), {}, (void*) &LSMap<int, double>::constructor},
		{Type::tmp_map(Type::integer, Type::integer), {}, (void*) &LSMap<int, int>::constructor},
	});

	operator_("in", {
		{Type::const_map(Type::any, Type::any), Type::any, Type::boolean, (void*) &LSMap<LSValue*, LSValue*>::in},
		{Type::const_map(Type::any, Type::real), Type::any, Type::boolean, (void*) &LSMap<LSValue*, double>::in},
		{Type::const_map(Type::any, Type::integer), Type::any, Type::boolean, (void*) &LSMap<LSValue*, int>::in},
		{Type::const_map(Type::real, Type::any), Type::real, Type::boolean, (void*) &LSMap<double, LSValue*>::in},
		{Type::const_map(Type::real, Type::real), Type::real, Type::boolean, (void*) &LSMap<double, double>::in},
		{Type::const_map(Type::real, Type::integer), Type::real, Type::boolean, (void*) &LSMap<double, int>::in},
		{Type::const_map(Type::integer, Type::any), Type::integer, Type::boolean, (void*) &LSMap<int, LSValue*>::in},
		{Type::const_map(Type::integer, Type::real), Type::integer, Type::boolean, (void*) &LSMap<int, double>::in},
		{Type::const_map(Type::integer, Type::integer), Type::long_, Type::boolean, (void*) &LSMap<int, int>::in},
	});

	method("size", {
		{Type::integer, {Type::const_map(Type::any, Type::any)}, (void*) map_size},
		{Type::integer, {Type::const_map(Type::any, Type::real)}, (void*) map_size},
		{Type::integer, {Type::const_map(Type::any, Type::integer)}, (void*) map_size},
		{Type::integer, {Type::const_map(Type::integer, Type::any)}, (void*) map_size},
		{Type::integer, {Type::const_map(Type::integer, Type::real)}, (void*) map_size},
		{Type::integer, {Type::const_map(Type::integer, Type::integer)}, (void*) map_size},
    });

	method("values", {
		{Type::array(Type::any), {Type::const_map(Type::any, Type::any)}, (void*) &LSMap<LSValue*, LSValue*>::values},
		{Type::array(Type::real), {Type::const_map(Type::any, Type::real)}, (void*) &LSMap<LSValue*, double>::values},
		{Type::array(Type::integer), {Type::const_map(Type::any, Type::integer)}, (void*) &LSMap<LSValue*, int>::values},
		{Type::array(Type::any), {Type::const_map(Type::real, Type::any)}, (void*) &LSMap<double, LSValue*>::values},
		{Type::array(Type::real), {Type::const_map(Type::real, Type::real)}, (void*) &LSMap<double, double>::values},
		{Type::array(Type::integer), {Type::const_map(Type::real, Type::integer)}, (void*) &LSMap<double, int>::values},
		{Type::array(Type::any), {Type::const_map(Type::integer, Type::any)}, (void*) &LSMap<int, LSValue*>::values},
		{Type::array(Type::real), {Type::const_map(Type::integer, Type::real)}, (void*) &LSMap<int, double>::values},
		{Type::array(Type::integer), {Type::const_map(Type::integer, Type::integer)}, (void*) &LSMap<int, int>::values}
	});

	method("insert", {
		{Type::boolean, {Type::map(Type::any, Type::any), Type::any, Type::any}, (void*) &LSMap<LSValue*, LSValue*>::ls_insert},
		{Type::boolean, {Type::map(Type::any, Type::real), Type::any, Type::real}, (void*) &LSMap<LSValue*, double>::ls_insert},
		{Type::boolean, {Type::map(Type::any, Type::integer), Type::any, Type::integer}, (void*) &LSMap<LSValue*, int>::ls_insert},
		{Type::boolean, {Type::map(Type::integer, Type::any), Type::integer, Type::any}, (void*) &LSMap<int, LSValue*>::ls_insert},
		{Type::boolean, {Type::map(Type::integer, Type::real), Type::integer, Type::real}, (void*) &LSMap<int, double>::ls_insert},
		{Type::boolean, {Type::map(Type::integer, Type::integer), Type::integer, Type::integer}, (void*) &LSMap<int, int>::ls_insert},
    });

	method("clear", {
		{Type::map(Type::any, Type::any), {Type::map()}, (void*) &LSMap<LSValue*,LSValue*>::ls_clear},
		{Type::map(Type::any, Type::real), {Type::map(Type::any, Type::real)}, (void*) &LSMap<LSValue*,double>::ls_clear},
		{Type::map(Type::any, Type::integer), {Type::map(Type::any, Type::integer)}, (void*) &LSMap<LSValue*,int>::ls_clear},
		{Type::map(Type::integer, Type::any), {Type::map(Type::integer, Type::any)}, (void*) &LSMap<int,LSValue*>::ls_clear},
		{Type::map(Type::integer, Type::real), {Type::map(Type::integer, Type::real)}, (void*) &LSMap<int,double>::ls_clear},
		{Type::map(Type::integer, Type::integer), {Type::map(Type::integer, Type::integer)}, (void*) &LSMap<int,int>::ls_clear},
	});

	method("erase", {
		{Type::boolean, {Type::map(Type::any, Type::any), Type::any}, (void*) &LSMap<LSValue*,LSValue*>::ls_erase},
		{Type::boolean, {Type::map(Type::any, Type::real), Type::any}, (void*) &LSMap<LSValue*,double>::ls_erase},
		{Type::boolean, {Type::map(Type::any, Type::integer), Type::any}, (void*) &LSMap<LSValue*,int>::ls_erase},
		{Type::boolean, {Type::map(Type::integer, Type::any), Type::integer}, (void*) &LSMap<int,LSValue*>::ls_erase},
		{Type::boolean, {Type::map(Type::integer, Type::real), Type::integer}, (void*) &LSMap<int,double>::ls_erase},
		{Type::boolean, {Type::map(Type::integer, Type::integer), Type::integer}, (void*) &LSMap<int,int>::ls_erase},
	});

	auto lK = Type::template_("K");
	auto lV = Type::template_("V");
	template_(lK, lV).
	method("look", {
		{lV, {Type::const_map(lK, lV), lK, lV}, look},
	});

	method("min", {
		{Type::any, {Type::const_map(Type::any, Type::any)}, (void*) &LSMap<LSValue*,LSValue*>::ls_min, THROWS},
		{Type::real, {Type::const_map(Type::any, Type::real)}, (void*) &LSMap<LSValue*,double>::ls_min, THROWS},
		{Type::integer, {Type::const_map(Type::any, Type::integer)}, (void*) &LSMap<LSValue*,int>::ls_min, THROWS},
		{Type::any, {Type::const_map(Type::integer, Type::any)}, (void*) &LSMap<int,LSValue*>::ls_min, THROWS},
		{Type::real, {Type::const_map(Type::integer, Type::real)}, (void*) &LSMap<int,double>::ls_min, THROWS},
		{Type::integer, {Type::const_map(Type::integer, Type::integer)}, (void*) &LSMap<int,int>::ls_min, THROWS},
	});

	method("minKey", {
		{Type::any, {Type::const_map(Type::any, Type::any)}, (void*) &LSMap<LSValue*,LSValue*>::ls_minKey, THROWS},
		{Type::any, {Type::const_map(Type::any, Type::real)}, (void*) &LSMap<LSValue*,double>::ls_minKey, THROWS},
		{Type::any, {Type::const_map(Type::any, Type::integer)}, (void*) &LSMap<LSValue*,int>::ls_minKey, THROWS},
		{Type::integer, {Type::const_map(Type::integer, Type::any)}, (void*) &LSMap<int,LSValue*>::ls_minKey, THROWS},
		{Type::integer, {Type::const_map(Type::integer, Type::real)}, (void*) &LSMap<int,double>::ls_minKey, THROWS},
		{Type::integer, {Type::const_map(Type::integer, Type::integer)}, (void*) &LSMap<int,int>::ls_minKey, THROWS},
	});

	method("max", {
		{Type::any, {Type::const_map(Type::any, Type::any)}, (void*) &LSMap<LSValue*,LSValue*>::ls_max, THROWS},
		{Type::real, {Type::const_map(Type::any, Type::real)}, (void*) &LSMap<LSValue*,double>::ls_max, THROWS},
		{Type::integer, {Type::const_map(Type::any, Type::integer)}, (void*) &LSMap<LSValue*,int>::ls_max, THROWS},
		{Type::any, {Type::const_map(Type::integer, Type::any)}, (void*) &LSMap<int,LSValue*>::ls_max, THROWS},
		{Type::real, {Type::const_map(Type::integer, Type::real)}, (void*) &LSMap<int,double>::ls_max, THROWS},
		{Type::integer, {Type::const_map(Type::integer, Type::integer)}, (void*) &LSMap<int,int>::ls_max, THROWS},
	});

	method("maxKey", {
		{Type::any, {Type::const_map(Type::any, Type::any)}, (void*) &LSMap<LSValue*,LSValue*>::ls_maxKey, THROWS},
		{Type::any, {Type::const_map(Type::any, Type::real)}, (void*) &LSMap<LSValue*,double>::ls_maxKey, THROWS},
		{Type::any, {Type::const_map(Type::any, Type::integer)}, (void*) &LSMap<LSValue*,int>::ls_maxKey, THROWS},
		{Type::integer, {Type::const_map(Type::integer, Type::any)}, (void*) &LSMap<int,LSValue*>::ls_maxKey, THROWS},
		{Type::integer, {Type::const_map(Type::integer, Type::real)}, (void*) &LSMap<int,double>::ls_maxKey, THROWS},
		{Type::integer, {Type::const_map(Type::integer, Type::integer)}, (void*) &LSMap<int,int>::ls_maxKey, THROWS},
	});

	auto iter_ptr = &LSMap<LSValue*, LSValue*>::ls_iter<LSFunction*>;
	auto iK = Type::template_("K");
	auto iV = Type::template_("V");
	template_(iK, iV).
	method("iter", {
		{Type::void_, {Type::const_map(Type::any, Type::any), Type::fun(Type::void_, {Type::any, Type::any})}, (void*) iter_ptr, THROWS},
		{Type::void_, {Type::const_map(iK, iV), Type::fun(Type::void_, {iK, iV})}, iter, THROWS},
	});

	auto flT = Type::template_("T");
	auto flK = Type::template_("K");
	auto flI = Type::template_("I");
	auto flR = Type::template_("R");
	template_(flT, flK, flI, flR).
	method("foldLeft", {
		{flR, {Type::const_map(flK, flT), Type::fun(flR, {Type::meta_add(flI, flR), flK, flT}), flI}, fold_left},
	});

	auto frT = Type::template_("T");
	auto frK = Type::template_("K");
	auto frI = Type::template_("I");
	auto frR = Type::template_("R");
	template_(frT, frK, frI, frR).
	method("foldRight", {
		{frR, {Type::const_map(frK, frT), Type::fun(frR, {frK, frT, Type::meta_add(frI, frR)}), frI}, fold_right},
	});

	/** Internal **/
	method("at", {
		{Type::integer, {Type::map(Type::integer, Type::integer), Type::integer}, (void*) &LSMap<int, int>::at},
		{Type::real, {Type::map(Type::integer, Type::real), Type::integer}, (void*) &LSMap<int, double>::at},
		{Type::any, {Type::map(Type::integer, Type::any), Type::integer}, (void*) &LSMap<int, LSValue*>::at},
		{Type::integer, {Type::map(Type::real, Type::integer), Type::real}, (void*) &LSMap<double, int>::at},
		{Type::real, {Type::map(Type::real, Type::real), Type::real}, (void*) &LSMap<double, double>::at},
		{Type::any, {Type::map(Type::real, Type::any), Type::real}, (void*) &LSMap<double, LSValue*>::at},
		{Type::integer, {Type::map(Type::any, Type::integer), Type::any}, (void*) &LSMap<LSValue*, int>::at},
		{Type::real, {Type::map(Type::any, Type::real), Type::any}, (void*) &LSMap<LSValue*, double>::at},
		{Type::any, {Type::map(Type::any, Type::any), Type::any}, (void*) &LSMap<LSValue*, LSValue*>::at},
	});
	method("insert_fun", {
		{Type::void_, {Type::map(Type::any, Type::any), Type::any, Type::any}, (void*) &LSMap<LSValue*, LSValue*>::ls_emplace},
		{Type::void_, {Type::map(Type::any, Type::real), Type::any, Type::real}, (void*) &LSMap<LSValue*, double>::ls_emplace},
		{Type::void_, {Type::map(Type::any, Type::integer), Type::any, Type::integer}, (void*) &LSMap<LSValue*, int>::ls_emplace},
		{Type::void_, {Type::map(Type::real, Type::any), Type::real, Type::any}, (void*) &LSMap<double, LSValue*>::ls_emplace},
		{Type::void_, {Type::map(Type::real, Type::real), Type::real, Type::real}, (void*) &LSMap<double, double>::ls_emplace},
		{Type::void_, {Type::map(Type::real, Type::integer), Type::real, Type::integer}, (void*) &LSMap<double, int>::ls_emplace},
		{Type::void_, {Type::map(Type::integer, Type::any), Type::integer, Type::any}, (void*) &LSMap<int, LSValue*>::ls_emplace},
		{Type::void_, {Type::map(Type::integer, Type::real), Type::integer, Type::real}, (void*) &LSMap<int, double>::ls_emplace},
		{Type::void_, {Type::map(Type::integer, Type::integer), Type::integer, Type::integer}, (void*) &LSMap<int, int>::ls_emplace},
	});
	method("atL", {
		{Type::void_, {Type::map(Type::any, Type::any), Type::any, Type::any}, (void*) &LSMap<LSValue*, LSValue*>::atL_base},
		{Type::void_, {Type::map(Type::any, Type::real), Type::any, Type::real}, (void*) &LSMap<LSValue*, double>::atL_base},
		{Type::void_, {Type::map(Type::any, Type::integer), Type::any, Type::integer}, (void*) &LSMap<LSValue*, int>::atL_base},
		{Type::void_, {Type::map(Type::real, Type::any), Type::real, Type::any}, (void*) &LSMap<double, LSValue*>::atL_base},
		{Type::void_, {Type::map(Type::real, Type::real), Type::real, Type::real}, (void*) &LSMap<double, double>::atL_base},
		{Type::void_, {Type::map(Type::real, Type::integer), Type::real, Type::integer}, (void*) &LSMap<double, int>::atL_base},
		{Type::void_, {Type::map(Type::integer, Type::any), Type::integer, Type::any}, (void*) &LSMap<int, LSValue*>::atL_base},
		{Type::void_, {Type::map(Type::integer, Type::real), Type::integer, Type::real}, (void*) &LSMap<int, double>::atL_base},
		{Type::void_, {Type::map(Type::integer, Type::integer), Type::integer, Type::integer}, (void*) &LSMap<int, int>::atL_base},
	});
	// std::map<int, int>::iterator (LSMap<int, int>::*mapend)() = &LSMap<int, int>::end;
	method("end", {
		{Type::map()->iterator(), {Type::map()}, (void*) end}
	});
	method("iterator_end", {
		{Type::map()->iterator(), {Type::map()}, (void*) iterator_end}
	});
	method("iterator_inc", {
		{Type::map()->iterator(), {Type::map()->iterator()}, (void*) iterator_inc}
	});
	method("iterator_dec", {
		{Type::map()->iterator(), {Type::map()->iterator()}, (void*) iterator_dec}
	});
	method("iterator_rkey", {
		{Type::i8_ptr, {Type::map()->iterator()}, (void*) iterator_rkey}
	});
	method("iterator_rget", {
		{Type::integer, {Type::map()->iterator()}, (void*) iterator_rget_ii},
		{Type::integer, {Type::map()->iterator()}, (void*) iterator_rget_vi},
		{Type::real, {Type::map()->iterator()}, (void*) iterator_rget_ir},
		{Type::any, {Type::map()->iterator()}, (void*) iterator_rget_vv},
	});
	method("look_fun", {
		{Type::any, {Type::const_map(Type::any, Type::any), Type::any, Type::any}, (void*) &LSMap<LSValue*, LSValue*>::ls_look},
		{Type::real, {Type::const_map(Type::any, Type::real), Type::any, Type::real}, (void*) &LSMap<LSValue*, double>::ls_look},
		{Type::integer, {Type::const_map(Type::any, Type::integer), Type::any, Type::integer}, (void*) &LSMap<LSValue*, int>::ls_look},
		{Type::any, {Type::const_map(Type::integer, Type::any), Type::integer, Type::any}, (void*) &LSMap<int, LSValue*>::ls_look},
		{Type::real, {Type::const_map(Type::integer, Type::real), Type::integer, Type::real}, (void*) &LSMap<int, double>::ls_look},
		{Type::integer, {Type::const_map(Type::integer, Type::integer), Type::integer, Type::integer}, (void*) &LSMap<int, int>::ls_look},
	});
}

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
	c.insn_foreach(args[0], Type::void_, v, k, [&](Compiler::value v, Compiler::value k) -> Compiler::value {
		c.insn_store(result->val, c.insn_call(function, {c.insn_load(result->val), k, v}));
		return {};
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
	c.insn_foreach(args[0], Type::void_, v, k, [&](Compiler::value v, Compiler::value k) -> Compiler::value {
		c.insn_store(result->val, c.insn_call(function, {k, v, c.insn_load(result->val)}));
		return {};
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
	c.insn_foreach(args[0], Type::void_, v, k, [&](Compiler::value v, Compiler::value k) -> Compiler::value {
		return c.insn_call(function, {k, v});
	});
	return {};
}

}
