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
int* iterator_rget_ii(std::map<int, int>::iterator it) {
	return &std::map<int, int>::reverse_iterator(it)->second;
}
int* iterator_rget_vi(std::map<void*, int>::iterator it) {
	return &std::map<void*, int>::reverse_iterator(it)->second;
}
double* iterator_rget_ir(std::map<int, double>::iterator it) {
	return &std::map<int, double>::reverse_iterator(it)->second;
}
void** iterator_rget_vv(std::map<void*, void*>::iterator it) {
	return &std::map<void*, void*>::reverse_iterator(it)->second;
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

	auto inK = env.template_("K");
	auto inV = env.template_("V");
	auto inT = env.template_("T");
	template_(inK, inV, inT).
	operator_("in", {
		{Type::const_map(inK, inV), inT, env.boolean, ADDR(in)}
	});

	method("size", {
		{env.integer, {Type::const_map(env.any, env.any)}, ADDR((void*) map_size)},
		{env.integer, {Type::const_map(env.any, env.real)}, ADDR((void*) map_size)},
		{env.integer, {Type::const_map(env.any, env.integer)}, ADDR((void*) map_size)},
		{env.integer, {Type::const_map(env.integer, env.any)}, ADDR((void*) map_size)},
		{env.integer, {Type::const_map(env.integer, env.real)}, ADDR((void*) map_size)},
		{env.integer, {Type::const_map(env.integer, env.integer)}, ADDR((void*) map_size)},
    });

	auto vK = env.template_("K");
	auto vV = env.template_("V");
	template_(vK, vV).
	method("values", {
		{Type::array(vV), {Type::const_map(vK, vV)}, ADDR(values)}
	});

	auto iK1 = env.template_("K1");
	auto iV1 = env.template_("V1");
	auto iK2 = env.template_("K2");
	auto iV2 = env.template_("V2");
	template_(iK1, iV1, iK2, iV2).
	method("insert", {
		{env.boolean, {Type::map(iK1, iV1), iK2, iV2}, ADDR(insert), 0, { }}
    });

	auto cK = env.template_("K");
	auto cV = env.template_("V");
	template_(cK, cV).
	method("clear", {
		{Type::map(env.never, env.never), {Type::map(cK, cV)}, ADDR(clear)}
	});

	auto eK = env.template_("K");
	auto eV = env.template_("V");
	auto eK2 = env.template_("K2");
	template_(eK, eV, eK2).
	method("erase", {
		{env.boolean, {Type::map(eK, eV), eK2}, ADDR(erase)},
	});

	auto lK = env.template_("K");
	auto lV = env.template_("V");
	auto lK2 = env.template_("K2");
	auto lV2 = env.template_("V2");
	template_(lK, lV, lK2, lV2).
	method("look", {
		{Type::meta_add(lV, lV2), {Type::const_map(lK, lV), lK2, lV2}, ADDR(look)},
	});

	auto mK = env.template_("K");
	auto mV = env.template_("V");
	template_(mK, mV).
	method("min", {
		{mV, {Type::const_map(mK, mV)}, ADDR(min), THROWS}
	});

	auto mkK = env.template_("K");
	auto mkV = env.template_("V");
	template_(mkK, mkV).
	method("minKey", {
		{mkK, {Type::const_map(mkK, mkV)}, ADDR(min_key), THROWS}
	});

	auto maK = env.template_("K");
	auto maV = env.template_("V");
	template_(maK, maV).
	method("max", {
		{maV, {Type::const_map(maK, maV)}, ADDR(max), THROWS}
	});

	auto makK = env.template_("K");
	auto makV = env.template_("V");
	template_(makK, makV).
	method("maxKey", {
		{makK, {Type::const_map(makK, makV)}, ADDR(max_key), THROWS}
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

	/**
	 * Internal
	 */
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
	}, PRIVATE);

	method("insert_fun", {
		{env.boolean, {Type::map(env.any, env.any), env.any, env.any}, ADDR(((void*) &LSMap<LSValue*, LSValue*>::ls_emplace))},
		{env.boolean, {Type::map(env.any, env.real), env.any, env.real}, ADDR(((void*) &LSMap<LSValue*, double>::ls_emplace))},
		{env.boolean, {Type::map(env.any, env.integer), env.any, env.integer}, ADDR(((void*) &LSMap<LSValue*, int>::ls_emplace))},
		{env.boolean, {Type::map(env.real, env.any), env.real, env.any}, ADDR(((void*) &LSMap<double, LSValue*>::ls_emplace))},
		{env.boolean, {Type::map(env.real, env.real), env.real, env.real}, ADDR(((void*) &LSMap<double, double>::ls_emplace))},
		{env.boolean, {Type::map(env.real, env.integer), env.real, env.integer}, ADDR(((void*) &LSMap<double, int>::ls_emplace))},
		{env.boolean, {Type::map(env.integer, env.any), env.integer, env.any}, ADDR(((void*) &LSMap<int, LSValue*>::ls_emplace))},
		{env.boolean, {Type::map(env.integer, env.real), env.integer, env.real}, ADDR(((void*) &LSMap<int, double>::ls_emplace))},
		{env.boolean, {Type::map(env.integer, env.integer), env.integer, env.integer}, ADDR(((void*) &LSMap<int, int>::ls_emplace))},
	}, PRIVATE);

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
	}, PRIVATE);

	// std::map<int, int>::iterator (LSMap<int, int>::*mapend)() = &LSMap<int, int>::end;
	method("end", {
		{Type::map(env.void_, env.void_)->iterator(), {Type::map(env.void_, env.void_)}, ADDR((void*) end)}
	}, PRIVATE);

	method("iterator_end", {
		{Type::map(env.void_, env.void_)->iterator(), {Type::map(env.void_, env.void_)}, ADDR((void*) iterator_end)}
	}, PRIVATE);

	method("iterator_inc", {
		{Type::map(env.void_, env.void_)->iterator(), {Type::map(env.void_, env.void_)->iterator()}, ADDR((void*) iterator_inc)}
	}, PRIVATE);

	method("iterator_dec", {
		{Type::map(env.void_, env.void_)->iterator(), {Type::map(env.void_, env.void_)->iterator()}, ADDR((void*) iterator_dec)}
	}, PRIVATE);

	method("iterator_rkey", {
		{env.i8_ptr, {Type::map(env.void_, env.void_)->iterator()}, ADDR((void*) iterator_rkey)}
	}, PRIVATE);

	method("iterator_rget", {
		{env.integer->pointer(), {Type::map(env.void_, env.void_)->iterator()}, ADDR((void*) iterator_rget_ii)},
		{env.integer->pointer(), {Type::map(env.void_, env.void_)->iterator()}, ADDR((void*) iterator_rget_vi)},
		{env.real->pointer(), {Type::map(env.void_, env.void_)->iterator()}, ADDR((void*) iterator_rget_ir)},
		{env.any->pointer(), {Type::map(env.void_, env.void_)->iterator()}, ADDR((void*) iterator_rget_vv)},
	}, PRIVATE);

	auto look_any_any = ADDR((&LSMap<LSValue*, LSValue*>::ls_look<LSValue*>));
	auto look_any_real = ADDR((&LSMap<LSValue*, double>::ls_look<LSValue*>));
	auto look_any_int_any = ADDR((&LSMap<LSValue*, int>::ls_look<LSValue*>));
	auto look_any_int_int = ADDR((&LSMap<LSValue*, int>::ls_look<int>));
	auto look_int_any = ADDR((&LSMap<int, LSValue*>::ls_look<LSValue*>));
	auto look_int_real = ADDR((&LSMap<int, double>::ls_look<double>));
	auto look_int_int = ADDR((&LSMap<int, int>::ls_look<int>));
	method("look_fun", {
		{env.any, {Type::const_map(env.any, env.any), env.any, env.any}, (void*) look_any_any},
		{env.real, {Type::const_map(env.any, env.real), env.any, env.real}, (void*) look_any_real},
		{env.any, {Type::const_map(env.any, env.integer), env.any, env.any}, (void*) look_any_int_any},
		{env.integer, {Type::const_map(env.any, env.integer), env.any, env.integer}, (void*) look_any_int_int},
		{env.any, {Type::const_map(env.integer, env.any), env.integer, env.any}, (void*) look_int_any},
		{env.real, {Type::const_map(env.integer, env.real), env.integer, env.real}, (void*) look_int_real},
		{env.integer, {Type::const_map(env.integer, env.integer), env.integer, env.integer}, (void*) look_int_int},
	}, PRIVATE);

	method("ls_insert_fun", {
		{env.boolean, {Type::map(env.any, env.any), env.any, env.any}, ADDR(((void*) &LSMap<LSValue*, LSValue*>::ls_insert))},
		{env.boolean, {Type::map(env.any, env.real), env.any, env.real}, ADDR(((void*) &LSMap<LSValue*, double>::ls_insert))},
		{env.boolean, {Type::map(env.any, env.integer), env.any, env.integer}, ADDR(((void*) &LSMap<LSValue*, int>::ls_insert))},
		{env.boolean, {Type::map(env.integer, env.any), env.integer, env.any}, ADDR(((void*) &LSMap<int, LSValue*>::ls_insert))},
		{env.boolean, {Type::map(env.integer, env.real), env.integer, env.real}, ADDR(((void*) &LSMap<int, double>::ls_insert))},
		{env.boolean, {Type::map(env.integer, env.integer), env.integer, env.integer}, ADDR(((void*) &LSMap<int, int>::ls_insert))},
	}, PRIVATE);

	method("clear_fun", {
		{Type::map(env.never, env.never), {Type::map(env.any, env.any)}, ADDR(((void*) &LSMap<LSValue*,LSValue*>::ls_clear))},
		{Type::map(env.never, env.never), {Type::map(env.any, env.real)}, ADDR(((void*) &LSMap<LSValue*,double>::ls_clear))},
		{Type::map(env.never, env.never), {Type::map(env.any, env.integer)}, ADDR(((void*) &LSMap<LSValue*,int>::ls_clear))},
		{Type::map(env.never, env.never), {Type::map(env.integer, env.any)}, ADDR(((void*) &LSMap<int,LSValue*>::ls_clear))},
		{Type::map(env.never, env.never), {Type::map(env.integer, env.real)}, ADDR(((void*) &LSMap<int,double>::ls_clear))},
		{Type::map(env.never, env.never), {Type::map(env.integer, env.integer)}, ADDR(((void*) &LSMap<int,int>::ls_clear))},
	}, PRIVATE);

	method("in_fun", {
		{Type::const_map(env.any, env.any), env.any, env.boolean, ADDR(((void*) &LSMap<LSValue*, LSValue*>::in))},
		{Type::const_map(env.any, env.real), env.any, env.boolean, ADDR(((void*) &LSMap<LSValue*, double>::in))},
		{Type::const_map(env.any, env.integer), env.any, env.boolean, ADDR(((void*) &LSMap<LSValue*, int>::in))},
		{Type::const_map(env.real, env.any), env.real, env.boolean, ADDR(((void*) &LSMap<double, LSValue*>::in))},
		{Type::const_map(env.real, env.real), env.real, env.boolean, ADDR(((void*) &LSMap<double, double>::in))},
		{Type::const_map(env.real, env.integer), env.real, env.boolean, ADDR(((void*) &LSMap<double, int>::in))},
		{Type::const_map(env.integer, env.any), env.integer, env.boolean, ADDR(((void*) &LSMap<int, LSValue*>::in))},
		{Type::const_map(env.integer, env.real), env.integer, env.boolean, ADDR(((void*) &LSMap<int, double>::in))},
		{Type::const_map(env.integer, env.integer), env.long_, env.boolean, ADDR(((void*) &LSMap<int, int>::in))},
	}, PRIVATE);

	method("values_fun", {
		{Type::array(env.any), {Type::const_map(env.any, env.any)}, ADDR(((void*) &LSMap<LSValue*, LSValue*>::values))},
		{Type::array(env.real), {Type::const_map(env.any, env.real)}, ADDR(((void*) &LSMap<LSValue*, double>::values))},
		{Type::array(env.integer), {Type::const_map(env.any, env.integer)}, ADDR(((void*) &LSMap<LSValue*, int>::values))},
		{Type::array(env.any), {Type::const_map(env.real, env.any)}, ADDR(((void*) &LSMap<double, LSValue*>::values))},
		{Type::array(env.real), {Type::const_map(env.real, env.real)}, ADDR(((void*) &LSMap<double, double>::values))},
		{Type::array(env.integer), {Type::const_map(env.real, env.integer)}, ADDR(((void*) &LSMap<double, int>::values))},
		{Type::array(env.any), {Type::const_map(env.integer, env.any)}, ADDR(((void*) &LSMap<int, LSValue*>::values))},
		{Type::array(env.real), {Type::const_map(env.integer, env.real)}, ADDR(((void*) &LSMap<int, double>::values))},
		{Type::array(env.integer), {Type::const_map(env.integer, env.integer)}, ADDR(((void*) &LSMap<int, int>::values))}
	}, PRIVATE);

	method("min_fun", {
		{env.any, {Type::const_map(env.any, env.any)}, ADDR(((void*) &LSMap<LSValue*,LSValue*>::ls_min)), THROWS},
		{env.real, {Type::const_map(env.any, env.real)}, ADDR(((void*) &LSMap<LSValue*,double>::ls_min)), THROWS},
		{env.integer, {Type::const_map(env.any, env.integer)}, ADDR(((void*) &LSMap<LSValue*,int>::ls_min)), THROWS},
		{env.any, {Type::const_map(env.integer, env.any)}, ADDR(((void*) &LSMap<int,LSValue*>::ls_min)), THROWS},
		{env.real, {Type::const_map(env.integer, env.real)}, ADDR(((void*) &LSMap<int,double>::ls_min)), THROWS},
		{env.integer, {Type::const_map(env.integer, env.integer)}, ADDR(((void*) &LSMap<int,int>::ls_min)), THROWS},
	}, PRIVATE);

	method("min_key_fun", {
		{env.any, {Type::const_map(env.any, env.any)}, ADDR(((void*) &LSMap<LSValue*,LSValue*>::ls_minKey)), THROWS},
		{env.any, {Type::const_map(env.any, env.real)}, ADDR(((void*) &LSMap<LSValue*,double>::ls_minKey)), THROWS},
		{env.any, {Type::const_map(env.any, env.integer)}, ADDR(((void*) &LSMap<LSValue*,int>::ls_minKey)), THROWS},
		{env.integer, {Type::const_map(env.integer, env.any)}, ADDR(((void*) &LSMap<int,LSValue*>::ls_minKey)), THROWS},
		{env.integer, {Type::const_map(env.integer, env.real)}, ADDR(((void*) &LSMap<int,double>::ls_minKey)), THROWS},
		{env.integer, {Type::const_map(env.integer, env.integer)}, ADDR(((void*) &LSMap<int,int>::ls_minKey)), THROWS},
	}, PRIVATE);

	method("max_fun", {
		{env.any, {Type::const_map(env.any, env.any)}, ADDR(((void*) &LSMap<LSValue*,LSValue*>::ls_max)), THROWS},
		{env.real, {Type::const_map(env.any, env.real)}, ADDR(((void*) &LSMap<LSValue*,double>::ls_max)), THROWS},
		{env.integer, {Type::const_map(env.any, env.integer)}, ADDR(((void*) &LSMap<LSValue*,int>::ls_max)), THROWS},
		{env.any, {Type::const_map(env.integer, env.any)}, ADDR(((void*) &LSMap<int,LSValue*>::ls_max)), THROWS},
		{env.real, {Type::const_map(env.integer, env.real)}, ADDR(((void*) &LSMap<int,double>::ls_max)), THROWS},
		{env.integer, {Type::const_map(env.integer, env.integer)}, ADDR(((void*) &LSMap<int,int>::ls_max)), THROWS},
	}, PRIVATE);

	method("max_key_fun", {
		{env.any, {Type::const_map(env.any, env.any)}, ADDR(((void*) &LSMap<LSValue*,LSValue*>::ls_maxKey)), THROWS},
		{env.any, {Type::const_map(env.any, env.real)}, ADDR(((void*) &LSMap<LSValue*,double>::ls_maxKey)), THROWS},
		{env.any, {Type::const_map(env.any, env.integer)}, ADDR(((void*) &LSMap<LSValue*,int>::ls_maxKey)), THROWS},
		{env.integer, {Type::const_map(env.integer, env.any)}, ADDR(((void*) &LSMap<int,LSValue*>::ls_maxKey)), THROWS},
		{env.integer, {Type::const_map(env.integer, env.real)}, ADDR(((void*) &LSMap<int,double>::ls_maxKey)), THROWS},
		{env.integer, {Type::const_map(env.integer, env.integer)}, ADDR(((void*) &LSMap<int,int>::ls_maxKey)), THROWS},
	}, PRIVATE);

	method("erase_fun", {
		{env.boolean, {Type::map(env.any, env.any), env.any}, ADDR(((void*) &LSMap<LSValue*,LSValue*>::ls_erase))},
		{env.boolean, {Type::map(env.any, env.real), env.any}, ADDR(((void*) &LSMap<LSValue*,double>::ls_erase))},
		{env.boolean, {Type::map(env.any, env.integer), env.any}, ADDR(((void*) &LSMap<LSValue*,int>::ls_erase))},
		{env.boolean, {Type::map(env.integer, env.any), env.integer}, ADDR(((void*) &LSMap<int,LSValue*>::ls_erase))},
		{env.boolean, {Type::map(env.integer, env.real), env.integer}, ADDR(((void*) &LSMap<int,double>::ls_erase))},
		{env.boolean, {Type::map(env.integer, env.integer), env.integer}, ADDR(((void*) &LSMap<int,int>::ls_erase))},
	}, PRIVATE);
}

#if COMPILER

Compiler::value MapSTD::in(Compiler& c, std::vector<Compiler::value> args, int) {
	auto map = args[0];
	auto f = [&]() {
		if (map.t->key()->is_integer()) {
			if (map.t->element()->is_integer()) return "Map.in_fun.8";
			if (map.t->element()->is_real()) return "Map.in_fun.7";
			return "Map.in_fun.6";
		}
		if (map.t->key()->is_real()) {
			if (map.t->element()->is_integer()) return "Map.in_fun.5";
			if (map.t->element()->is_real()) return "Map.in_fun.4";
			return "Map.in_fun.3";
		}
		if (map.t->element()->is_integer()) return "Map.in_fun.2";
		if (map.t->element()->is_real()) return "Map.in_fun.1";
		return "Map.in_fun";
	}();
	return c.insn_call(c.env.boolean, args, f);
}

Compiler::value MapSTD::look(Compiler& c, std::vector<Compiler::value> args, int) {
	auto map = args[0];
	auto v = args[2];
	auto f = [&]() {
		if (map.t->key()->is_integer()) {
			if (map.t->element()->is_integer()) return "Map.look_fun.6";
			if (map.t->element()->is_real()) return "Map.look_fun.5";
			return "Map.look_fun.4";
		}
		if (map.t->element()->is_integer()) {
			if (v.t->is_integer()) return "Map.look_fun.3";
			return "Map.look_fun.2";
		}
		if (map.t->element()->is_real()) return "Map.look_fun.1";
		return "Map.look_fun";
	}();
	auto return_type = v.t->not_temporary()->operator + (map.t->element());
	auto key = c.insn_convert(args[1], map.t->key());
	auto value = c.insn_convert(v, return_type);
	return c.insn_convert(c.insn_call(return_type, {args[0], key, value}, f), return_type);
}

Compiler::value MapSTD::insert(Compiler& c, std::vector<Compiler::value> args, int) {
	auto map = args[0];
	auto f = [&]() {
		if (map.t->key()->is_integer()) {
			if (map.t->element()->is_integer()) return "Map.ls_insert_fun.5";
			if (map.t->element()->is_real()) return "Map.ls_insert_fun.4";
			return "Map.ls_insert_fun.3";
		}
		if (map.t->element()->is_integer()) return "Map.ls_insert_fun.2";
		if (map.t->element()->is_real()) return "Map.ls_insert_fun.1";
		return "Map.ls_insert_fun";
	}();
	auto key = c.insn_convert(args[1], map.t->key());
	auto value = c.insn_convert(args[2], map.t->element());
	return c.insn_call(map.t->element(), {args[0], key, value}, f);
}

Compiler::value MapSTD::clear(Compiler& c, std::vector<Compiler::value> args, int) {
	auto map = args[0];
	auto f = [&]() {
		if (map.t->key()->is_integer()) {
			if (map.t->element()->is_integer()) return "Map.clear_fun.5";
			if (map.t->element()->is_real()) return "Map.clear_fun.4";
			return "Map.clear_fun.3";
		}
		if (map.t->element()->is_integer()) return "Map.clear_fun.2";
		if (map.t->element()->is_real()) return "Map.clear_fun.1";
		return "Map.clear_fun";
	}();
	return c.insn_call(Type::map(c.env.never, c.env.never), args, f);
}

Compiler::value MapSTD::fold_left(Compiler& c, std::vector<Compiler::value> args, int) {
	auto function = args[1];
	auto init_type = function.t->argument(0);
	auto result = Variable::new_temporary("r", init_type);
	result.create_entry(c);
	c.insn_store(result.entry, c.insn_convert(c.insn_move(args[2]), init_type));
	// c.add_temporary_variable(&result);
	auto v = Variable::new_temporary("v", args[0].t->element());
	auto k = Variable::new_temporary("k", args[0].t->key());
	// v->create_entry(c);
	// k->create_entry(c);
	// c.add_temporary_variable(v);
	// c.add_temporary_variable(k);
	c.insn_foreach(args[0], c.env.void_, &v, &k, [&](Compiler::value v, Compiler::value k) -> Compiler::value {
		c.insn_store(result.entry, c.insn_call(function, {c.insn_load(result.entry), k, v}));
		return { c.env };
	});
	return c.insn_load(result.entry);
}

Compiler::value MapSTD::fold_right(Compiler& c, std::vector<Compiler::value> args, int) {
	auto function = args[1];
	auto init_type = function.t->argument(2);
	auto result = Variable::new_temporary("r", init_type);
	result.create_entry(c);
	c.insn_store(result.entry, c.insn_convert(c.insn_move(args[2]), init_type));
	// c.add_temporary_variable(&result);
	auto v = Variable::new_temporary("v", args[0].t->element());
	auto k = Variable::new_temporary("k", args[0].t->key());
	// v->create_entry(c);
	// k->create_entry(c);
	// c.add_temporary_variable(v);
	// c.add_temporary_variable(k);
	c.insn_foreach(args[0], c.env.void_, &v, &k, [&](Compiler::value v, Compiler::value k) -> Compiler::value {
		c.insn_store(result.entry, c.insn_call(function, {k, v, c.insn_load(result.entry)}));
		return { c.env };
	}, true);
	return c.insn_load(result.entry);
}

Compiler::value MapSTD::iter(Compiler& c, std::vector<Compiler::value> args, int) {
	auto function = args[1];
	auto v = Variable::new_temporary("v", args[0].t->element());
	auto k = Variable::new_temporary("k", args[0].t->key());
	// v->create_entry(c);
	// k->create_entry(c);
	// c.add_temporary_variable(v);
	// c.add_temporary_variable(k);
	c.insn_foreach(args[0], c.env.void_, &v, &k, [&](Compiler::value v, Compiler::value k) -> Compiler::value {
		return c.insn_call(function, {k, v});
	});
	return { c.env };
}

Compiler::value MapSTD::min(Compiler& c, std::vector<Compiler::value> args, int) {
	auto map = args[0];
	auto f = [&]() {
		if (map.t->key()->is_integer()) {
			if (map.t->element()->is_integer()) return "Map.min_fun.5";
			if (map.t->element()->is_real()) return "Map.min_fun.4";
			return "Map.min_fun.3";
		}
		if (map.t->element()->is_integer()) return "Map.min_fun.2";
		if (map.t->element()->is_real()) return "Map.min_fun.1";
		return "Map.min_fun";
	}();
	return c.insn_invoke(args[0].t->element(), args, f);
}

Compiler::value MapSTD::min_key(Compiler& c, std::vector<Compiler::value> args, int) {
	auto map = args[0];
	auto f = [&]() {
		if (map.t->key()->is_integer()) {
			if (map.t->element()->is_integer()) return "Map.min_key_fun.5";
			if (map.t->element()->is_real()) return "Map.min_key_fun.4";
			return "Map.min_fun.3";
		}
		if (map.t->element()->is_integer()) return "Map.min_key_fun.2";
		if (map.t->element()->is_real()) return "Map.min_key_fun.1";
		return "Map.min_key_fun";
	}();
	return c.insn_invoke(args[0].t->key(), args, f);
}

Compiler::value MapSTD::max(Compiler& c, std::vector<Compiler::value> args, int) {
	auto map = args[0];
	auto f = [&]() {
		if (map.t->key()->is_integer()) {
			if (map.t->element()->is_integer()) return "Map.max_fun.5";
			if (map.t->element()->is_real()) return "Map.max_fun.4";
			return "Map.max_fun.3";
		}
		if (map.t->element()->is_integer()) return "Map.max_fun.2";
		if (map.t->element()->is_real()) return "Map.max_fun.1";
		return "Map.max_fun";
	}();
	return c.insn_invoke(args[0].t->element(), args, f);
}

Compiler::value MapSTD::max_key(Compiler& c, std::vector<Compiler::value> args, int) {
	auto map = args[0];
	auto f = [&]() {
		if (map.t->key()->is_integer()) {
			if (map.t->element()->is_integer()) return "Map.max_key_fun.5";
			if (map.t->element()->is_real()) return "Map.max_key_fun.4";
			return "Map.max_key_fun.3";
		}
		if (map.t->element()->is_integer()) return "Map.max_key_fun.2";
		if (map.t->element()->is_real()) return "Map.max_key_fun.1";
		return "Map.max_key_fun";
	}();
	return c.insn_invoke(args[0].t->key(), args, f);
}

Compiler::value MapSTD::erase(Compiler& c, std::vector<Compiler::value> args, int) {
	auto map = args[0];
	auto f = [&]() {
		if (map.t->key()->is_integer()) {
			if (map.t->element()->is_integer()) return "Map.erase_fun.5";
			if (map.t->element()->is_real()) return "Map.erase_fun.4";
			return "Map.erase_fun.3";
		}
		if (map.t->element()->is_integer()) return "Map.erase_fun.2";
		if (map.t->element()->is_real()) return "Map.erase_fun.1";
		return "Map.erase_fun";
	}();
	auto key = c.insn_convert(args[1], args[0].t->key());
	return c.insn_invoke(args[0].t->key(), {args[0], key}, f);
}

Compiler::value MapSTD::values(Compiler& c, std::vector<Compiler::value> args, int) {
	auto map = args[0];
	auto f = [&]() {
		if (map.t->key()->is_integer()) {
			if (map.t->element()->is_integer()) return "Map.values_fun.8";
			if (map.t->element()->is_real()) return "Map.values_fun.7";
			return "Map.values_fun.6";
		}
		if (map.t->key()->is_real()) {
			if (map.t->element()->is_integer()) return "Map.values_fun.5";
			if (map.t->element()->is_real()) return "Map.values_fun.4";
			return "Map.values_fun.3";
		}
		if (map.t->element()->is_integer()) return "Map.values_fun.2";
		if (map.t->element()->is_real()) return "Map.values_fun.1";
		return "Map.values_fun";
	}();
	return c.insn_call(Type::array(args[0].t->element()), args, f);
}

#endif

}
