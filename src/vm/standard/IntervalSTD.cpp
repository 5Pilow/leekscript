#include "IntervalSTD.hpp"
#include "ValueSTD.hpp"
#include "../value/LSInterval.hpp"
#include "../value/LSClosure.hpp"
#include "../../type/Type.hpp"
#include "ArraySTD.hpp"

namespace ls {

IntervalSTD::IntervalSTD(VM* vm) : Module(vm, "Interval") {

	#if COMPILER
	LSInterval::clazz = lsclass.get();
	#endif

	constructor_({
		{Type::tmp_interval, {Type::integer, Type::integer}, (void*) &LSInterval::constructor}
	});

	/*
	 * Operators
	 */
	operator_("in", {
		{Type::interval, Type::integer, Type::boolean, (void*) &LSInterval::in_i}
	});

	auto ttR = Type::template_("R");
	template_(ttR).
	operator_("~~", {
		{Type::tmp_array(ttR), {Type::const_interval, Type::fun(ttR, {Type::integer})}, ADDR(ArraySTD::map)},
	});

	/*
	 * Methods
	 */
	method("copy", {
		{Type::interval, {Type::interval}, ADDR(ValueSTD::copy)}
	});
	auto pred_fun_type_int = Type::fun_object(Type::boolean, {Type::integer});
	auto pred_clo_type_int = Type::closure(Type::boolean, {Type::integer});
	auto filter_fun = &LSInterval::ls_filter<LSFunction*>;
	auto filter_clo = &LSInterval::ls_filter<LSClosure*>;
	method("filter", {
		{Type::tmp_array(Type::integer), {Type::interval, pred_fun_type_int}, (void*) filter_fun},
		{Type::tmp_array(Type::integer), {Type::interval, pred_clo_type_int}, (void*) filter_clo}
	});

	auto mapR = Type::template_("R");
	template_(mapR).
	method("map", {
		{Type::tmp_array(mapR), {Type::const_interval, Type::fun(mapR, {Type::integer})}, ADDR(ArraySTD::map)},
	});

	method("sum", {
		{Type::long_, {Type::interval}, (void*) &LSInterval::ls_sum},
	});
	method("product", {
		{Type::long_, {Type::interval}, (void*) &LSInterval::ls_product},
	});

	/** Interval **/
	method("atv", {
		{Type::boolean, {Type::interval, Type::integer}, (void*) &LSInterval::atv}
	});
}

IntervalSTD::~IntervalSTD() {}

}
