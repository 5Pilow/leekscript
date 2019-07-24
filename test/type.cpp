#include "Test.hpp"
#include "../src/colors.h"
#include "../src/type/Array_type.hpp"
#include "../src/type/Function_type.hpp"

void Test::test_types() {

	auto p1 = ls::Type::generate_new_placeholder_type();

	auto vm = getVM(false);

	header("Types");
	section("JSON name");
	test("long", ls::Type::long_->getJsonName(), "number");
	test("any", ls::Type::map()->getJsonName(), "map");
	test("interval", ls::Type::interval->getJsonName(), "interval");
	test("object", ls::Type::object->getJsonName(), "object");
	test("class", ls::Type::clazz()->getJsonName(), "class");
	test("set", ls::Type::set()->getJsonName(), "set");
	test("null", ls::Type::null->getJsonName(), "null");
	test("fun", ls::Type::fun()->getJsonName(), "function");

	section("print");
	auto pt = [&](const ls::Type* type) {
		std::ostringstream oss;
		oss << type;
		return oss.str();
	};
	test("void", pt(ls::Type::void_), C_GREY "void" END_COLOR);
	test("any", pt(ls::Type::any), BLUE_BOLD "any" END_COLOR);
	test("null", pt(ls::Type::null), BLUE_BOLD "null" END_COLOR);
	test("bool", pt(ls::Type::boolean), BLUE_BOLD "bool" END_COLOR);
	test("integer", pt(ls::Type::integer), BLUE_BOLD "int" END_COLOR);
	test("real", pt(ls::Type::real), BLUE_BOLD "real" END_COLOR);
	test("long", pt(ls::Type::long_), BLUE_BOLD "long" END_COLOR);
	test("mpz", pt(ls::Type::mpz), BLUE_BOLD "mpz" END_COLOR);
	test("array<int>", pt(ls::Type::array(ls::Type::integer)), BLUE_BOLD "array" END_COLOR "<" BLUE_BOLD "int" END_COLOR ">");
	test("array<real>", pt(ls::Type::array(ls::Type::real)), BLUE_BOLD "array" END_COLOR "<" BLUE_BOLD "real" END_COLOR ">");
	test("array<any>", pt(ls::Type::array(ls::Type::any)), BLUE_BOLD "array" END_COLOR "<" BLUE_BOLD "any" END_COLOR ">");
	test("object", pt(ls::Type::object), BLUE_BOLD "object" END_COLOR);
	//assert_print(ls::Type::array(ls::Type::integer).iterator(), BLUE_BOLD "iterator" END_COLOR "<" BLUE_BOLD "array" END_COLOR "<" BLUE_BOLD "int" END_COLOR ">>");
	test("integer | string", pt(ls::Type::compound({ls::Type::integer, ls::Type::string})), BLUE_BOLD "int" END_COLOR " | " BLUE_BOLD "string" END_COLOR);

	section("operator ==");
	test("void", ls::Type::void_, ls::Type::void_);
	test("any", ls::Type::any, ls::Type::any);
	test("integer", ls::Type::integer, ls::Type::integer);
	test("array<int>", ls::Type::array(ls::Type::integer), ls::Type::array(ls::Type::integer));
	test("array<real>", ls::Type::array(ls::Type::real), ls::Type::array(ls::Type::real));
	test("map<any, any>", ls::Type::map(ls::Type::any, ls::Type::any), ls::Type::map(ls::Type::any, ls::Type::any));
	test("integer | string", ls::Type::compound({ls::Type::integer, ls::Type::string}), ls::Type::compound({ls::Type::integer, ls::Type::string}));
	test("(-> null).return", ls::Type::fun(ls::Type::null, {})->return_type(), ls::Type::null);
	test("mpz*", ls::Type::mpz->pointer(), ls::Type::mpz_ptr);
	test("mpz*.pointed", ls::Type::mpz_ptr->pointed(), ls::Type::mpz);
	test("mpz*&&", ls::Type::mpz->pointer()->add_temporary(), ls::Type::tmp_mpz_ptr);
	assert(ls::Type::mpz->add_temporary()->pointer() == ls::Type::tmp_mpz_ptr);
	assert(ls::Type::mpz_ptr->add_temporary() == ls::Type::tmp_mpz_ptr);
	assert(ls::Type::tmp_mpz->pointer() == ls::Type::tmp_mpz_ptr);
	assert(ls::Type::tmp_mpz_ptr->pointed() == ls::Type::mpz);
	assert(ls::Type::tmp_mpz->not_temporary() == ls::Type::mpz);
	assert(ls::Type::string->add_temporary() == ls::Type::tmp_string);
	assert(ls::Type::tmp_string->not_temporary() == ls::Type::string);
	assert(ls::Type::mpz->add_constant()->add_temporary() == ls::Type::tmp_mpz);

	section("is_array");
	test("void", ls::Type::void_->is_array(), false);
	test("any", ls::Type::any->is_array(), false);
	test("integer", ls::Type::integer->is_array(), false);
	test("array", ls::Type::array()->is_array(), true);
	test("array<any>", ls::Type::array(ls::Type::any)->is_array(), true);
	test("array<integer>", ls::Type::array(ls::Type::integer)->is_array(), true);
	test("array<real>", ls::Type::array(ls::Type::real)->is_array(), true);
	test("array<integer> | array<real>", ls::Type::compound({ ls::Type::array(ls::Type::integer), ls::Type::array(ls::Type::real) })->is_array(), true);
	test("array<integer> | set<real>", ls::Type::compound({ ls::Type::array(ls::Type::integer), ls::Type::set(ls::Type::real) })->is_array(), false);

	section("castable");
	test("real -> int", ls::Type::real->castable(ls::Type::integer), true);
	test("int -> real", ls::Type::integer->castable(ls::Type::real), true);
	test("array<int> -> array<int>", ls::Type::array(ls::Type::integer)->castable(ls::Type::array(ls::Type::integer)), true);
	test("array<int> -> array", ls::Type::array(ls::Type::integer)->castable(ls::Type::array()), true);
	test("array<real> -> array", ls::Type::array(ls::Type::real)->castable(ls::Type::array()), true);
	test("map<any, any> -> map", ls::Type::map(ls::Type::any, ls::Type::any)->castable(ls::Type::map()), true);
	test("map<integer, any> -> map", ls::Type::map(ls::Type::integer, ls::Type::any)->castable(ls::Type::map()), true);
	test("map<real, any> -> map", ls::Type::map(ls::Type::real, ls::Type::any)->castable(ls::Type::map()), true);
	test("any -> p1", ls::Type::any->castable(p1), true);
	test("number -> boolean", ls::Type::number->castable(ls::Type::boolean), true);
	test("boolean -> number", ls::Type::boolean->castable(ls::Type::number), true);
	test("array<real> -> any", ls::Type::array(ls::Type::real)->castable(ls::Type::any), true);
	test("array<real> -> const:any", ls::Type::array(ls::Type::real)->castable(ls::Type::const_any), true);

	section("strictCastable");
	test("int -> real", ls::Type::integer->strictCastable(ls::Type::real), true);
	test("real -> int", ls::Type::real->strictCastable(ls::Type::integer), false);

	section("Distance");
	test("number <=> any", ls::Type::number->distance(ls::Type::any), 1);
	test("mpz* <=> any", ls::Type::mpz_ptr->distance(ls::Type::any), 2);
	test("integer | real <=> real", ls::Type::compound({ ls::Type::integer, ls::Type::real })->distance(ls::Type::real), 0);
	test("real <=> integer | real", ls::Type::real->distance(ls::Type::compound({ ls::Type::integer, ls::Type::real })), 0);
	test("integer | real <=> integer | real", ls::Type::compound({ ls::Type::integer, ls::Type::real })->distance(ls::Type::compound({ ls::Type::integer, ls::Type::real })), 0);
	test("const:string <=> string&&", ls::Type::const_string->distance(ls::Type::tmp_string), -1);
	test("string <=> number", ls::Type::string->distance(ls::Type::number), -1);
	test("p.element <=> any", p1->element()->distance(ls::Type::any), 0);
	test("any <=> p.element", ls::Type::any->element()->distance(p1->element()), 0);

	section("element()");
	test("array<int>.element", ls::Type::array(ls::Type::integer)->element(), ls::Type::integer);
	test("array<p>.element", ls::Type::array(p1)->element(), p1);
	test("array<int | p>.element", ls::Type::array(ls::Type::compound({ ls::Type::integer, p1 }))->element(), ls::Type::compound({ ls::Type::integer, p1 }));
	test("(array<int> | array<real>).element", ls::Type::compound({ ls::Type::array(ls::Type::integer), ls::Type::array(ls::Type::real) })->element(), ls::Type::compound({ ls::Type::integer, ls::Type::real }));

	section("placeholder");
	test("array<p.element>", ls::Type::array(p1->element()), p1);

	section("is_number");
	test("number", ls::Type::number->is_number(), true);
	test("long", ls::Type::long_->is_number(), true);
	test("mpz", ls::Type::mpz->is_number(), true);
	test("integer", ls::Type::integer->is_number(), true);
	test("real", ls::Type::real->is_number(), true);
	test("boolean", ls::Type::boolean->is_number(), true);
	test("string", ls::Type::string->is_number(), false);
	test("any", ls::Type::any->is_number(), false);
	test("array", ls::Type::array()->is_number(), false);
	test("map", ls::Type::map()->is_number(), false);
	test("void", ls::Type::void_->is_number(), false);

	section("callable()");
	test("any", ls::Type::any->callable(), true);
	test("fun", ls::Type::fun()->callable(), true);
	test("class", ls::Type::clazz()->callable(), true);
	test("integer", ls::Type::integer->callable(), false);
	test("array", ls::Type::array()->callable(), false);

	section("container()");
	test("array", ls::Type::array()->container(), true);
	test("string", ls::Type::string->container(), true);
	test("set", ls::Type::set()->container(), true);
	test("interval", ls::Type::interval->container(), true);
	test("map", ls::Type::map()->container(), true);
	test("object", ls::Type::object->container(), true);
	test("array | string", ls::Type::compound({ls::Type::string, ls::Type::array()})->container(), true);

	section("operator +");
	test("void + int", ls::Type::void_->operator + (ls::Type::integer), ls::Type::integer);
	test("int + real", ls::Type::integer->operator + (ls::Type::real), ls::Type::compound({ls::Type::integer, ls::Type::real}));
	test("never + int", ls::Type::never->operator + (ls::Type::integer), ls::Type::integer);
	test("int + never", ls::Type::integer->operator + (ls::Type::never), ls::Type::integer);
	test("array<never> + array<int>", ls::Type::array(ls::Type::never)->operator + (ls::Type::array(ls::Type::integer)), ls::Type::array(ls::Type::integer));
	test("array<int> + array<never>", ls::Type::array(ls::Type::integer)->operator + (ls::Type::array(ls::Type::never)), ls::Type::array(ls::Type::integer));
	test("array<int>&& + array<real>", ls::Type::tmp_array(ls::Type::integer)->operator + (ls::Type::array(ls::Type::real)), ls::Type::tmp_compound({ ls::Type::array(ls::Type::integer), ls::Type::array(ls::Type::real) }));
	test("array<int>&& + array<real>&&", ls::Type::tmp_array(ls::Type::integer)->operator + (ls::Type::tmp_array(ls::Type::real)), ls::Type::tmp_compound({ ls::Type::array(ls::Type::integer), ls::Type::array(ls::Type::real) }));

	section("operator *");
	test("void * void", ls::Type::void_->operator * (ls::Type::void_), ls::Type::void_);
	test("any * any", ls::Type::any->operator * (ls::Type::any), ls::Type::any);
	test("void * int", ls::Type::void_->operator * (ls::Type::integer), ls::Type::integer);
	test("any * int", ls::Type::any->operator * (ls::Type::integer), ls::Type::any);
	test("int * real", ls::Type::integer->operator * (ls::Type::real), ls::Type::real);
	test("int * string", ls::Type::integer->operator * (ls::Type::string), ls::Type::any);
	test("int * bool", ls::Type::integer->operator * (ls::Type::boolean), ls::Type::any);
	test("any * fun", ls::Type::any->operator * (ls::Type::fun(ls::Type::void_, {})), ls::Type::any);
	test("array<int> * array<real>", ls::Type::array(ls::Type::integer)->operator * (ls::Type::array(ls::Type::real)), ls::Type::any);
	test("int * fun", ls::Type::integer->operator * (ls::Type::fun()), ls::Type::any);
	test("never * int", ls::Type::never->operator * (ls::Type::integer), ls::Type::integer);
	test("array<never> * array<int>", ls::Type::array(ls::Type::never)->operator * (ls::Type::array(ls::Type::integer)), ls::Type::array(ls::Type::integer));

	section("fold");
	test("void.fold()", ls::Type::void_->fold(), ls::Type::void_);
	test("null.fold()", ls::Type::null->fold(), ls::Type::null);
	test("{int}.fold()", ls::Type::compound({ls::Type::integer})->fold(), ls::Type::integer);
	test("{int, int}.fold()", ls::Type::compound({ls::Type::integer, ls::Type::integer})->fold(), ls::Type::integer);
	test("{int, real}.fold()", ls::Type::compound({ls::Type::integer, ls::Type::real})->fold(), ls::Type::real);
	test("{array<int>, array<real>}.fold()", ls::Type::compound({ls::Type::array(ls::Type::integer), ls::Type::array(ls::Type::real)})->fold(), ls::Type::any);
	test("string&&.fold()", ls::Type::tmp_string->fold(), ls::Type::tmp_string);
	test("mpz.fold", ls::Type::mpz->fold(), ls::Type::mpz);
	test("mpz*.fold", ls::Type::mpz_ptr->fold(), ls::Type::mpz_ptr);
	test("mpz&&.fold", ls::Type::tmp_mpz->fold(), ls::Type::tmp_mpz);
	test("mpz*&&.fold", ls::Type::tmp_mpz_ptr->fold(), ls::Type::tmp_mpz_ptr);

	section("temporary");
	test("int&&", ls::Type::integer->add_temporary(), ls::Type::integer);
	test("string&&", ls::Type::string->add_temporary(), ls::Type::tmp_string);
	test("(int | string)&&.fold", ls::Type::compound({ls::Type::integer, ls::Type::string})->add_temporary()->fold(), ls::Type::any->add_temporary());
	test("(const:int)&&", ls::Type::integer->add_constant()->add_temporary(), ls::Type::integer->add_constant());
	test("(array<int> | array<real>)&&", ls::Type::compound({ ls::Type::array(ls::Type::integer), ls::Type::array(ls::Type::real) })->add_temporary(), ls::Type::tmp_compound({ ls::Type::array(ls::Type::integer), ls::Type::array(ls::Type::real) }));

	section("LLVM type");
	test("void", ls::Type::void_->llvm(vm->compiler), llvm::Type::getVoidTy(vm->compiler.getContext()));
	test("integer", ls::Type::integer->llvm(vm->compiler), llvm::Type::getInt32Ty(vm->compiler.getContext()));
	test("boolean", ls::Type::boolean->llvm(vm->compiler), llvm::Type::getInt1Ty(vm->compiler.getContext()));
	test("real", ls::Type::real->llvm(vm->compiler), llvm::Type::getDoubleTy(vm->compiler.getContext()));
	test("integer | real", ls::Type::compound({ls::Type::integer, ls::Type::real})->llvm(vm->compiler), llvm::Type::getDoubleTy(vm->compiler.getContext()));
	test("string&&", ls::Type::string->add_temporary()->llvm(vm->compiler), ls::Type::string->llvm(vm->compiler));

	section("Program type");
	code("").type(ls::Type::void_);
	code("null").type(ls::Type::null);
	code("12").type(ls::Type::integer);
	code("12.5").type(ls::Type::real);
	code("'salut'").type(ls::Type::tmp_string);
	code("[]").type(ls::Type::tmp_array(ls::Type::never));
	code("[1]").type(ls::Type::tmp_array(ls::Type::integer));
	code("[1, 2.5]").type(ls::Type::tmp_array(ls::Type::compound({ls::Type::integer, ls::Type::real})));
	code("['a']").type(ls::Type::tmp_array(ls::Type::string));
	code("[[1]]").type(ls::Type::tmp_array(ls::Type::array(ls::Type::compound({ ls::Type::integer, ls::Type::any }))));
	code("[[1, 2.5]]").type(ls::Type::tmp_array(ls::Type::array(ls::Type::compound({ls::Type::integer, ls::Type::real, ls::Type::any}))));
	code("[['a']]").type(ls::Type::tmp_array(ls::Type::array(ls::Type::compound({ls::Type::string, ls::Type::any}))));

	if (success_count != total) assert(false && "Type tests failed!");
}
