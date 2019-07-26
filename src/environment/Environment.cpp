#include "Environment.hpp"

namespace ls {

Environment() :
    void_(new Void_type {}),
    boolean(new Bool_type {}),
    const_boolean(Environment::boolean->add_constant()),
    number(new Number_type {}),
    const_number(Environment::number->add_constant()),
    i8(new I8_type {}),
    i8_ptr(i8->pointer()),
    integer(new Integer_type {}),
    const_integer(Environment::integer->add_constant()),
    long_(new Long_type {}),
    const_long(Environment::long_->add_constant()),
    mpz(new Mpz_type {}),
    tmp_mpz(Environment::mpz->add_temporary()),
    const_mpz(Environment::mpz->add_constant()),
    mpz_ptr(Environment::mpz->pointer()),
    tmp_mpz_ptr(Environment::mpz_ptr->add_temporary()),
    const_mpz_ptr(Environment::mpz_ptr->add_constant()),
    never(new Never_type {}),
    any(new Any_type {}),
    tmp_any(any->add_temporary()),
    const_any(any->add_constant()),
    null(new Null_type {}),
    string(new String_type {}),
    tmp_string(Environment::string->add_temporary()),
    const_string(Environment::string->add_constant()),
    real(new Real_type {}),
    const_real(Environment::real->add_constant()),
    interval(new Interval_type {}),
    const_interval(Environment::interval->add_constant()),
    tmp_interval(Environment::interval->add_temporary()),
    object(new Object_type {}),
    tmp_object(Environment::object->add_temporary())
{

}


const Type* Environment::generate_new_placeholder_type() {
	u_int32_t character = 0x03B1 + placeholder_counter;
	char buff[5];
	u8_toutf8(buff, 5, &character, 1);
	auto type = new Placeholder_type(std::string { buff });
	placeholder_counter++;
	Environment::placeholder_types.push_back(type);
	return type;
}

const Type* Environment::array() {
	return array(void_);
}
const Type* Environment::array(const Type* element) {
	if (auto e = dynamic_cast<const Meta_element_type*>(element)) return e->type;
	element = element->not_temporary();
	auto i = array_types.find(element);
	if (i != array_types.end()) return i->second;
	auto type = new Array_type(element);
	type->placeholder = element->placeholder;
	array_types.insert({element, type});
	return type;
}
const Type* Environment::const_array(const Type* element) {
	if (auto e = dynamic_cast<const Meta_element_type*>(element)) return e->type;
	auto i = const_array_types.find(element);
	if (i != const_array_types.end()) return i->second;
	auto type = array(element)->add_constant();
	const_array_types.insert({element, type});
	return type;
}
const Type* Environment::tmp_array(const Type* element) {
	if (auto e = dynamic_cast<const Meta_element_type*>(element)) return e->type;
	auto i = tmp_array_types.find(element);
	if (i != tmp_array_types.end()) return i->second;
	auto type = array(element)->add_temporary();
	tmp_array_types.insert({element, type});
	return type;
}
const Type* Environment::set(const Type* element) {
	auto i = set_types.find(element);
	if (i != set_types.end()) return i->second;
	auto type = new Set_type(element);
	set_types.insert({element, type});
	return type;
}
const Type* Environment::const_set(const Type* element) {
	auto i = const_set_types.find(element);
	if (i != const_set_types.end()) return i->second;
	auto type = set(element)->add_constant();
	const_set_types.insert({element, type});
	return type;
}
const Type* Environment::tmp_set(const Type* element) {
	auto i = tmp_set_types.find(element);
	if (i != tmp_set_types.end()) return i->second;
	auto type = set(element)->add_temporary();
	tmp_set_types.insert({element, type});
	return type;
}
const Type* Environment::map(const Type* key, const Type* element) {
	auto i = map_types.find({key, element});
	if (i != map_types.end()) return i->second;
	auto type = new Map_type(key, element);
	map_types.insert({{key, element}, type});
	return type;
}
const Type* Environment::const_map(const Type* key, const Type* element) {
	auto i = const_map_types.find({key, element});
	if (i != const_map_types.end()) return i->second;
	auto type = map(key, element)->add_constant();
	const_map_types.insert({{key, element}, type});
	return type;
}
const Type* Environment::tmp_map(const Type* key, const Type* element) {
	auto i = tmp_map_types.find({key, element});
	if (i != tmp_map_types.end()) return i->second;
	auto type = map(key, element)->add_temporary();
	tmp_map_types.insert({{key, element}, type});
	return type;
}
const Type* Environment::fun() {
    return fun(void_);
}
const Type* Environment::fun(const Type* return_type, std::vector<const Type*> arguments, const Value* function) {
	if (function == nullptr) {
		std::pair<const Type*, std::vector<const Type*>> key { return_type, arguments };
		auto i = function_types.find(key);
		if (i != function_types.end()) return i->second;
		auto type = new Function_type(return_type, arguments);
		type->constant = true;
		function_types.insert({ key, type });
		return type;
	} else {
		auto t = new Function_type(return_type, arguments, function);
		t->constant = true;
		return t;
	}
}
const Type* Environment::fun_object(const Type* return_type, std::vector<const Type*> arguments, const Value* function) {
	if (function == nullptr) {
		std::pair<const Type*, std::vector<const Type*>> key { return_type, arguments };
		auto i = function_object_types.find(key);
		if (i != function_object_types.end()) return i->second;
		auto type = new Function_object_type(return_type, arguments);
		type->constant = true;
		function_object_types.insert({ key, type });
		return type;
	} else {
		auto t = new Function_object_type(return_type, arguments, false, function);
		t->constant = true;
		return t;
	}
}
const Type* Environment::closure(const Type* return_type, std::vector<const Type*> arguments, const Value* function) {
	if (function == nullptr) {
		std::pair<const Type*, std::vector<const Type*>> key { return_type, arguments };
		auto i = closure_types.find(key);
		if (i != closure_types.end()) return i->second;
		auto type = new Function_object_type(return_type, arguments, true);
		type->constant = true;
		closure_types.insert({ key, type });
		return type;
	} else {
		auto t = new Function_object_type(return_type, arguments, true, function);
		t->constant = true;
		return t;
	}
}
const Type* Environment::structure(const std::string name, std::initializer_list<const Type*> types) {
	auto i = structure_types.find(name);
	if (i != structure_types.end()) return i->second;
	auto type = new Struct_type(name, types);
	structure_types.insert({ name, type });
	return type;
}
const Type* Environment::clazz(const std::string name) {
	auto i = class_types.find(name);
	if (i != class_types.end()) return i->second;
	auto type = new Class_type(name);
	class_types.insert({ name, type });
	return type;
}
const Type* Environment::const_class(const std::string name) {
	return clazz(name)->add_constant();
}
const Type* Environment::template_(std::string name) {
	return new Template_type(name);
}
const Type* Environment::compound(std::initializer_list<const Type*> types) {
	return Environment::compound(std::vector<const Type*>(types));
}
const Type* Environment::compound(std::vector<const Type*> types) {
	if (types.size() == 1) return *types.begin();
	std::set<const Type*> base;
	auto folded = Environment::void_;
	auto temporary = false;
	for (const auto& t : types) {
		if (auto c = dynamic_cast<const Compound_type*>(t)) {
			for (const auto& bt : c->types) base.insert(bt);
		} else {
			base.insert(t->not_temporary());
		}
		// std::cout << "compound t " << t << " tmp " << t->temporary << std::endl;
		temporary |= t->temporary;
		folded = folded->operator * (t);
	}
	// std::cout << "temporary compound " << temporary << std::endl;
	if (base.size() == 1) return *types.begin();
	auto i = compound_types.find(base);
	if (i != compound_types.end()) {
		if (temporary) {
			return i->second->add_temporary();
		}
		return i->second;
	}
	auto type = new Compound_type { base, folded };
	compound_types.insert({ base, type });
	if (temporary) {
		// std::cout << "temporary" << std::endl;
		return type->add_temporary();
	}
	return type;
}
const Type* Environment::tmp_compound(std::initializer_list<const Type*> types) {
	return compound(types)->add_temporary();
}

const Type* Environment::meta_add(const Type* t1, const Type* t2) {
	return new Meta_add_type(t1, t2);
}
const Type* Environment::meta_mul(const Type* t1, const Type* t2) {
	return new Meta_mul_type(t1, t2);
}
const Type* Environment::meta_base_of(const Type* type, const Type* base) {
	return new Meta_baseof_type(type, base);
}
const Type* Environment::meta_not_temporary(const Type* type) {
	return new Meta_not_temporary_type(type);
}

}