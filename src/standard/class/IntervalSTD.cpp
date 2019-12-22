#include "IntervalSTD.hpp"
#include "ValueSTD.hpp"
#include "../../type/Type.hpp"
#include "ArraySTD.hpp"
#include "../../environment/Environment.hpp"
#if COMPILER
#include "../../vm/value/LSInterval.hpp"
#include "../../vm/value/LSClosure.hpp"
#endif

namespace ls {

IntervalSTD::IntervalSTD(Environment& env) : Module(env, "Interval") {

	#if COMPILER
	env.interval_class = std::make_unique<LSClass>(clazz.get());
	lsclass = env.interval_class.get();
	#endif

	constructor_({
		{env.tmp_interval, {env.integer, env.integer}, ADDR((void*) &LSInterval::constructor)}
	});

	/*
	 * Operators
	 */
	operator_("in", {
		{env.interval, env.integer, env.boolean, ADDR((void*) &LSInterval::in_i)}
	});

	auto ttR = env.template_("R");
	template_(ttR).
	operator_("~~", {
		{Type::tmp_array(ttR), {env.const_interval, Type::fun(ttR, {env.integer})}, ADDR(ArraySTD::map)},
	});

	/*
	 * Methods
	 */
	auto pred_fun_type_int = Type::fun_object(env.boolean, {env.integer});
	auto pred_clo_type_int = Type::closure(env.boolean, {env.integer});
	auto filter_fun = ADDR(&LSInterval::ls_filter<LSFunction*>);
	auto filter_clo = ADDR(&LSInterval::ls_filter<LSClosure*>);
	method("filter", {
		{Type::tmp_array(env.integer), {env.interval, pred_fun_type_int}, (void*) filter_fun},
		{Type::tmp_array(env.integer), {env.interval, pred_clo_type_int}, (void*) filter_clo}
	});

	auto mapR = env.template_("R");
	template_(mapR).
	method("map", {
		{Type::tmp_array(Type::meta_not_void(mapR)), {env.const_interval, Type::fun(mapR, {env.integer})}, ADDR(ArraySTD::map)},
	});

	method("sum", {
		{env.long_, {env.interval}, ADDR((void*) &LSInterval::ls_sum)},
	});
	method("product", {
		{env.long_, {env.interval}, ADDR((void*) &LSInterval::ls_product)},
	});

	/** Interval **/
	method("at_i_i", {
		{env.integer, {env.interval, env.integer}, ADDR((void*) &LSInterval::at_i_i)}
	}, PRIVATE);
}

IntervalSTD::~IntervalSTD() {}

}
