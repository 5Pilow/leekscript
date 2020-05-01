#include "Test.hpp"
#include "../src/colors.h"
#include "../src/type/Array_type.hpp"
#include "../src/type/Function_type.hpp"

void Test::test_types() {

	auto& env = getEnv();

	auto p1 = env.generate_new_placeholder_type();

	header("Types");
	section("JSON name");
	test("long", env.long_->getName(), "long");
	test("any", ls::Type::map(env.void_, env.void_)->getName(), "map");
	test("interval", env.interval->getName(), "interval");
	test("object", env.object->getName(), "object");
	test("class", env.clazz()->getName(), "class");
	test("set", env.set->getName(), "set");
	test("null", env.null->getName(), "null");
	test("fun", ls::Type::fun(env.void_, {})->getName(), "function");

	section("print");
	auto pt = [&](const ls::Type* type) {
		std::ostringstream oss;
		oss << type;
		return oss.str();
	};
	test("void", pt(env.void_), C_GREY "void" END_COLOR);
	test("any", pt(env.any), BLUE_BOLD "any" END_COLOR);
	test("null", pt(env.null), BLUE_BOLD "null" END_COLOR);
	test("bool", pt(env.boolean), BLUE_BOLD "bool" END_COLOR);
	test("integer", pt(env.integer), BLUE_BOLD "int" END_COLOR);
	test("real", pt(env.real), BLUE_BOLD "real" END_COLOR);
	test("long", pt(env.long_), BLUE_BOLD "long" END_COLOR);
	test("mpz", pt(env.mpz), BLUE_BOLD "mpz" END_COLOR);
	test("array<int>", pt(ls::Type::array(env.integer)), BLUE_BOLD "array" END_COLOR "<" BLUE_BOLD "int" END_COLOR ">");
	test("array<real>", pt(ls::Type::array(env.real)), BLUE_BOLD "array" END_COLOR "<" BLUE_BOLD "real" END_COLOR ">");
	test("array<any>", pt(ls::Type::array(env.any)), BLUE_BOLD "array" END_COLOR "<" BLUE_BOLD "any" END_COLOR ">");
	test("object", pt(env.object), BLUE_BOLD "object" END_COLOR);
	//assert_print(ls::Type::array(env.integer).iterator(), BLUE_BOLD "iterator" END_COLOR "<" BLUE_BOLD "array" END_COLOR "<" BLUE_BOLD "int" END_COLOR ">>");
	test("integer | string", pt(ls::Type::compound({env.integer, env.string})), BLUE_BOLD "int" END_COLOR " | " BLUE_BOLD "string" END_COLOR);

	section("operator ==");
	test("void", env.void_, env.void_);
	test("any", env.any, env.any);
	test("integer", env.integer, env.integer);
	test("array<int>", ls::Type::array(env.integer), ls::Type::array(env.integer));
	test("array<real>", ls::Type::array(env.real), ls::Type::array(env.real));
	test("map<any, any>", ls::Type::map(env.any, env.any), ls::Type::map(env.any, env.any));
	test("integer | string", ls::Type::compound({env.integer, env.string}), ls::Type::compound({env.integer, env.string}));
	test("(-> null).return", ls::Type::fun(env.null, {})->return_type(), env.null);
	test("mpz*", env.mpz->pointer(), env.mpz_ptr);
	test("mpz*.pointed", env.mpz_ptr->pointed(), env.mpz);
	test("mpz*&&", env.mpz->pointer()->add_temporary(), env.tmp_mpz_ptr);
	assert(env.mpz->add_temporary()->pointer() == env.tmp_mpz_ptr);
	assert(env.mpz_ptr->add_temporary() == env.tmp_mpz_ptr);
	assert(env.tmp_mpz->pointer() == env.tmp_mpz_ptr);
	assert(env.tmp_mpz_ptr->pointed() == env.mpz);
	assert(env.tmp_mpz->not_temporary() == env.mpz);
	assert(env.string->add_temporary() == env.tmp_string);
	assert(env.tmp_string->not_temporary() == env.string);
	assert(env.mpz->add_constant()->add_temporary() == env.tmp_mpz);

	section("is_array");
	test("void", env.void_->is_array(), false);
	test("any", env.any->is_array(), false);
	test("integer", env.integer->is_array(), false);
	test("array", env.array->is_array(), true);
	test("array<any>", ls::Type::array(env.any)->is_array(), true);
	test("array<integer>", ls::Type::array(env.integer)->is_array(), true);
	test("array<real>", ls::Type::array(env.real)->is_array(), true);
	test("array<integer> | array<real>", ls::Type::compound({ ls::Type::array(env.integer), ls::Type::array(env.real) })->is_array(), true);
	test("array<integer> | set<real>", ls::Type::compound({ ls::Type::array(env.integer), ls::Type::set(env.real) })->is_array(), false);

	section("castable");
	test("real -> int", env.real->castable(env.integer), true);
	test("int -> real", env.integer->castable(env.real), true);
	test("array<int> -> array<int>", ls::Type::array(env.integer)->castable(ls::Type::array(env.integer)), true);
	test("array<int> -> array", ls::Type::array(env.integer)->castable(env.array), true);
	test("array<real> -> array", ls::Type::array(env.real)->castable(env.array), true);
	test("map<any, any> -> map", ls::Type::map(env.any, env.any)->castable(ls::Type::map(env.void_, env.void_)), true);
	test("map<integer, any> -> map", ls::Type::map(env.integer, env.any)->castable(ls::Type::map(env.void_, env.void_)), true);
	test("map<real, any> -> map", ls::Type::map(env.real, env.any)->castable(ls::Type::map(env.void_, env.void_)), true);
	test("any -> p1", env.any->castable(p1), true);
	test("number -> boolean", env.number->castable(env.boolean), true);
	test("boolean -> number", env.boolean->castable(env.number), true);
	test("array<real> -> any", ls::Type::array(env.real)->castable(env.any), true);
	test("array<real> -> const:any", ls::Type::array(env.real)->castable(env.const_any), true);

	section("strictCastable");
	test("int -> real", env.integer->strictCastable(env.real), true);
	test("real -> int", env.real->strictCastable(env.integer), false);

	section("Distance");
	test("number <=> any", env.number->distance(env.any), 1);
	test("mpz* <=> any", env.mpz_ptr->distance(env.any), 2);
	test("integer | real <=> real", ls::Type::compound({ env.integer, env.real })->distance(env.real), 0);
	test("real <=> integer | real", env.real->distance(ls::Type::compound({ env.integer, env.real })), 0);
	test("integer | real <=> integer | real", ls::Type::compound({ env.integer, env.real })->distance(ls::Type::compound({ env.integer, env.real })), 0);
	test("const:string <=> string&&", env.const_string->distance(env.tmp_string), -1);
	test("string <=> number", env.string->distance(env.number), -1);
	test("p.element <=> any", p1->element()->distance(env.any), 0);
	test("any <=> p.element", env.any->element()->distance(p1->element()), 0);

	section("element()");
	test("array<int>.element", ls::Type::array(env.integer)->element(), env.integer);
	test("array<p>.element", ls::Type::array(p1)->element(), p1);
	test("array<int | p>.element", ls::Type::array(ls::Type::compound({ env.integer, p1 }))->element(), ls::Type::compound({ env.integer, p1 }));
	test("(array<int> | array<real>).element", ls::Type::compound({ ls::Type::array(env.integer), ls::Type::array(env.real) })->element(), ls::Type::compound({ env.integer, env.real }));

	section("placeholder");
	test("array<p.element>", ls::Type::array(p1->element()), p1);

	section("is_number");
	test("number", env.number->is_number(), true);
	test("long", env.long_->is_number(), true);
	test("mpz", env.mpz->is_number(), true);
	test("integer", env.integer->is_number(), true);
	test("real", env.real->is_number(), true);
	test("boolean", env.boolean->is_number(), true);
	test("string", env.string->is_number(), false);
	test("any", env.any->is_number(), false);
	test("array", env.array->is_number(), false);
	test("map", ls::Type::map(env.void_, env.void_)->is_number(), false);
	test("void", env.void_->is_number(), false);

	section("callable()");
	test("any", env.any->callable(), true);
	test("fun", ls::Type::fun(env.void_, {})->callable(), true);
	test("class", env.clazz()->callable(), true);
	test("integer", env.integer->callable(), false);
	test("array", env.array->callable(), false);

	section("container()");
	test("array", env.array->container(), true);
	test("string", env.string->container(), true);
	test("set", env.set->container(), true);
	test("interval", env.interval->container(), true);
	test("map", ls::Type::map(env.void_, env.void_)->container(), true);
	test("object", env.object->container(), true);
	test("array | string", ls::Type::compound({env.string, env.array})->container(), true);

	section("operator +");
	test("void + int", env.void_->operator + (env.integer), env.integer);
	test("int + real", env.integer->operator + (env.real), ls::Type::compound({env.integer, env.real}));
	test("never + int", env.never->operator + (env.integer), env.integer);
	test("int + never", env.integer->operator + (env.never), env.integer);
	test("array<never> + array<int>", ls::Type::array(env.never)->operator + (ls::Type::array(env.integer)), ls::Type::array(env.integer));
	test("array<int> + array<never>", ls::Type::array(env.integer)->operator + (ls::Type::array(env.never)), ls::Type::array(env.integer));
	test("array<int>&& + array<real>", ls::Type::tmp_array(env.integer)->operator + (ls::Type::array(env.real)), ls::Type::tmp_array(ls::Type::compound({ env.integer, env.real })));
	test("array<int>&& + array<real>&&", ls::Type::tmp_array(env.integer)->operator + (ls::Type::tmp_array(env.real)), ls::Type::tmp_array(ls::Type::compound({ env.integer, env.real })));
	test("array<array<int>> + array<array<never>>", ls::Type::array(ls::Type::array(env.integer))->operator + (ls::Type::array(ls::Type::array(env.never))), ls::Type::array(ls::Type::array(env.integer)));
	test("map<int, int> + map<never, never>", ls::Type::map(env.integer, env.integer)->operator + (ls::Type::map(env.never, env.never)), ls::Type::map(env.integer, env.integer));

	section("operator *");
	test("void * void", env.void_->operator * (env.void_), env.void_);
	test("any * any", env.any->operator * (env.any), env.any);
	test("void * int", env.void_->operator * (env.integer), env.integer);
	test("any * int", env.any->operator * (env.integer), env.any);
	test("int * real", env.integer->operator * (env.real), env.real);
	test("int * string", env.integer->operator * (env.string), env.any);
	test("int * bool", env.integer->operator * (env.boolean), env.any);
	test("any * fun", env.any->operator * (ls::Type::fun(env.void_, {})), env.any);
	test("array<int> * array<real>", ls::Type::array(env.integer)->operator * (ls::Type::array(env.real)), env.any);
	test("int * fun", env.integer->operator * (ls::Type::fun(env.void_, {})), env.any);
	test("never * int", env.never->operator * (env.integer), env.integer);
	test("array<never> * array<int>", ls::Type::array(env.never)->operator * (ls::Type::array(env.integer)), ls::Type::array(env.integer));

	section("fold");
	test("void.fold()", env.void_->fold(), env.void_);
	test("null.fold()", env.null->fold(), env.null);
	test("{int}.fold()", ls::Type::compound({env.integer})->fold(), env.integer);
	test("{int, int}.fold()", ls::Type::compound({env.integer, env.integer})->fold(), env.integer);
	test("{int, real}.fold()", ls::Type::compound({env.integer, env.real})->fold(), env.real);
	test("{array<int>, array<real>}.fold()", ls::Type::compound({ls::Type::array(env.integer), ls::Type::array(env.real)})->fold(), env.any);
	test("string&&.fold()", env.tmp_string->fold(), env.tmp_string);
	test("mpz.fold", env.mpz->fold(), env.mpz);
	test("mpz*.fold", env.mpz_ptr->fold(), env.mpz_ptr);
	test("mpz&&.fold", env.tmp_mpz->fold(), env.tmp_mpz);
	test("mpz*&&.fold", env.tmp_mpz_ptr->fold(), env.tmp_mpz_ptr);

	section("temporary");
	test("int&&", env.integer->add_temporary(), env.integer);
	test("string&&", env.string->add_temporary(), env.tmp_string);
	test("(int | string)&&.fold", ls::Type::compound({env.integer, env.string})->add_temporary()->fold(), env.any->add_temporary());
	test("(const:int)&&", env.integer->add_constant()->add_temporary(), env.integer->add_constant());
	test("(array<int> | array<real>)&&", ls::Type::compound({ ls::Type::array(env.integer), ls::Type::array(env.real) })->add_temporary(), ls::Type::tmp_compound({ ls::Type::array(env.integer), ls::Type::array(env.real) }));

	section("LLVM type");
	// test("void", env.void_->llvm(), llvm::Type::getVoidTy(vm->compiler.getContext()));
	// test("integer", env.integer->llvm(vm->compiler), llvm::Type::getInt32Ty(vm->compiler.getContext()));
	// test("boolean", env.boolean->llvm(vm->compiler), llvm::Type::getInt1Ty(vm->compiler.getContext()));
	// test("real", env.real->llvm(vm->compiler), llvm::Type::getDoubleTy(vm->compiler.getContext()));
	// test("integer | real", ls::Type::compound({env.integer, env.real})->llvm(vm->compiler), llvm::Type::getDoubleTy(vm->compiler.getContext()));
	// test("string&&", env.string->add_temporary()->llvm(vm->compiler), env.string->llvm(vm->compiler));

	section("Program type");
	code("").type(env.void_);
	code("null").type(env.null);
	code("12").type(env.integer);
	code("12.5").type(env.real);
	code("'salut'").type(env.tmp_string);
	code("[]").type(ls::Type::tmp_array(env.never));
	code("[1]").type(ls::Type::tmp_array(env.integer));
	code("[1, 2.5]").type(ls::Type::tmp_array(ls::Type::compound({env.integer, env.real})));
	code("['a']").type(ls::Type::tmp_array(env.string));
	code("[[1]]").type(ls::Type::tmp_array(ls::Type::array(env.integer)));
	code("[[1, 2.5]]").type(ls::Type::tmp_array(ls::Type::array(ls::Type::compound({env.integer, env.real}))));
	code("[['a']]").type(ls::Type::tmp_array(ls::Type::array(env.string)));

	// if (success_count != total) assert(false && "Type tests failed!");
}
