#include <sstream>
#include <numeric>
#include <algorithm>
#include <memory>
#include "Type.hpp"
#include "../constants.h"
#include "../colors.h"
#include "../util/utf8.h"
#include "Void_type.hpp"
#include "Any_type.hpp"
#include "Array_type.hpp"
#include "Fixed_array_type.hpp"
#include "Set_type.hpp"
#include "Function_type.hpp"
#include "Function_object_type.hpp"
#include "Class_type.hpp"
#include "Object_type.hpp"
#include "Class_type.hpp"
#include "Interval_type.hpp"
#include "String_type.hpp"
#include "Number_type.hpp"
#include "Long_type.hpp"
#include "Bool_type.hpp"
#include "Mpz_type.hpp"
#include "Real_type.hpp"
#include "Map_type.hpp"
#include "Null_type.hpp"
#include "Integer_type.hpp"
#include "I8_type.hpp"
#include "Placeholder_type.hpp"
#include "Struct_type.hpp"
#include "Pointer_type.hpp"
#include "Template_type.hpp"
#include "Never_type.hpp"
#include "Compound_type.hpp"
#include "Meta_add_type.hpp"
#include "Meta_concat_type.hpp"
#include "Meta_mul_type.hpp"
#include "Meta_baseof_type.hpp"
#include "Meta_element_type.hpp"
#include "Meta_temporary_type.hpp"
#include "Meta_not_temporary_type.hpp"
#include "Meta_not_void_type.hpp"
#include "../analyzer/value/Function.hpp"
#include "../analyzer/value/Value.hpp"
#include "../environment/Environment.hpp"

namespace ls {

unsigned int Type::placeholder_counter = 0;
const std::vector<const Type*> Type::empty_types;

Type::Type(Environment& env, bool native) : env(env), native(native) {
	folded = this;
}

const Type* Type::return_type() const {
	return env.any;
}
const Type* Type::argument(size_t) const {
	return env.any;
}
const std::vector<const Type*>& Type::arguments() const {
	return Type::empty_types;
}
const Type* Type::element() const {
	return env.any;
}
const Type* Type::element(size_t index) const {
	return env.any;
}
const std::vector<const Type*>& Type::elements() const {
	return Type::empty_types;
}
size_t Type::size() const {
	return 0u;
}
const Type* Type::key() const {
	return env.any;
}
const Type* Type::member(int) const {
	return env.any;
}

bool Type::must_manage_memory() const {
	if (is_void()) { return false; }
	return is_polymorphic() and not native;
}

const Type* Type::operator + (const Type* type) const {
	if (is_void() or is_never()) return type;
	if (type->is_void() or type->is_never()) return this;

	if (is_array() and type->is_array()) {
		bool tmp = temporary or type->temporary;
		if (tmp) {
			return Type::tmp_array(element()->operator + (type->element()));
		} else {
			return Type::array(element()->operator + (type->element()));
		}
		// const auto& sum = element()->operator + (type->element());
		// if (sum == element()) return this;
		// if (sum == type->element()) return type;
	}
	if (is_set() and type->is_set()) {
		const auto& sum = element()->operator + (type->element());
		if (sum == element()) return this;
		if (sum == type->element()) return type;
	}
	if (is_map() and type->is_map()) {
		bool tmp = temporary or type->temporary;
		return (tmp ? Type::tmp_map : Type::map)(key()->operator + (type->key()), element()->operator + (type->element()));
	}
	if (is_array() and type->is_map()) {
		if (element() == env.never) {
			return type;
		}
	}
	if (is_map() and type->is_array()) {
		if (type->element() == env.never) {
			return this;
		}
	}
	return Type::compound({this, type});
}
const Type* Type::operator * (const Type* t2) const {
	auto a = this->fold();
	auto b = t2->fold();
	if (a == env.void_ or a == env.never) return b;
	if (b == env.void_ or b == env.never or a == b) return a;
	if (a->is_polymorphic() and b->is_primitive()) {
		return env.any;
	}
	if (b->is_polymorphic() and a->is_primitive()) {
		return env.any;
	}
	if (a->is_bool() or b->is_bool()) {
		return env.any;
	}
	auto d1 = a->distance(b);
	auto d2 = b->distance(a);
	if (d1 >= 0 and d1 < 100000 and d2 >= 0 and d2 < 100000) {
		if (d1 < d2) return b;
		else return a;
	}
	return env.any;
}

const Type* Type::concat(const Type* type) const {
	if (is_fixed_array()) {
		auto new_elements = elements();
		if (type->is_fixed_array()) {
			for (const auto& element : type->elements()) {
				new_elements.push_back(element);
			}
		} else if (type->is_array()) {
			return tmp_array(element()->operator + (type->element()));
		}
		return tmp_fixed_array(new_elements);
	} else if (is_array()) {
		if (type->is_fixed_array()) {
			auto new_elements = elements();
			for (const auto& element : type->elements()) {
				new_elements.push_back(element);
			}
			return tmp_fixed_array(new_elements);
		} else {
			return tmp_array(element()->operator + (type));
		}
	}
	return operator + (type);
}

const Type* Type::fold() const {
	return folded;
}

std::string Type::to_string() const {
	std::ostringstream oss;
	oss << this;
	return oss.str();
}

const Type* Type::add_temporary() const {
	if (placeholder) return this;
	if (temporary) return this;
	if (is_primitive()) return this;
	if (constant) return not_constant()->add_temporary();
	auto i = env.temporary_types.find(this);
	if (i != env.temporary_types.end()) return i->second.get();
	auto type = this->clone();
	type->temporary = true;
	if (this != folded) {
		type->folded = type->folded->add_temporary();
	} else {
		type->folded = type;
	}
	env.temporary_types.emplace(this, type);
	env.not_temporary_types.insert({ type, this });
	return type;
}
const Type* Type::not_temporary() const {
	if (placeholder) return this;
	if (not temporary) return this;
	auto i = env.not_temporary_types.find(this);
	if (i != env.not_temporary_types.end()) return i->second;
	assert(false);
}
const Type* Type::add_constant() const {
	if (placeholder) return this;
	if (constant) return this;
	if (temporary) return not_temporary()->add_constant();
	auto i = env.const_types.find(this);
	if (i != env.const_types.end()) return i->second.get();
	auto type = this->clone();
	type->constant = true;
	if (this != folded) {
		type->folded = type->folded->add_constant();
	} else {
		type->folded = type;
	}
	env.const_types.emplace(this, type);
	env.not_const_types.insert({ type, this });
	return type;
}
const Type* Type::not_constant() const {
	if (placeholder) return this;
	if (not constant) return this;
	auto i = env.not_const_types.find(this);
	if (i != env.not_const_types.end()) return i->second;
	assert(false);
}

const Type* Type::pointer() const {
	auto i = env.pointer_types.find(this);
	if (i != env.pointer_types.end()) return i->second;
	if (temporary) {
		auto type = this->not_temporary()->pointer()->add_temporary();
		env.pointer_types.insert({ this, type });
		return type;
	} else if (constant) {
		auto type = this->not_constant()->pointer()->add_constant();
		env.pointer_types.insert({ this, type });
		return type;
	} else {
		auto type = new Pointer_type(this);
		env.base_pointer_types.emplace(this, type);
		env.pointer_types.insert({ this, type });
		return type;
	}
}

template <class T>
bool Type::is_type() const {
	if (dynamic_cast<const T*>(this) != nullptr) return true;
	if (auto t = dynamic_cast<const Template_type*>(this)) {
		return t->_implementation->is_type<T>();
	}
	if (auto c = dynamic_cast<const Compound_type*>(this)) {
		return c->all([&](const Type* t) {
			return dynamic_cast<const T*>(t) != nullptr;
		});
	}
	return false;
}
template <class T> bool Type::can_be_type() const {
	if (dynamic_cast<const T*>(this) != nullptr) return true;
	if (auto t = dynamic_cast<const Template_type*>(this)) {
		return t->_implementation->is_type<T>();
	}
	if (auto c = dynamic_cast<const Compound_type*>(this)) {
		return c->some([&](const Type* t) {
			return dynamic_cast<const T*>(t) != nullptr;
		});
	}
	return false;
}
bool Type::is_any() const { return is_type<Any_type>(); }
bool Type::is_bool() const { return folded == env.boolean or folded == env.const_boolean; }
bool Type::can_be_bool() const { return can_be_type<Bool_type>(); }
bool Type::is_number() const { return castable(env.number, true); }
bool Type::can_be_number() const {
	if (auto c = dynamic_cast<const Compound_type*>(this)) {
		return c->some([&](const Type* type) {
			return type->distance(env.number) >= 0;
		});
	}
	return distance(env.number) >= 0;
}
bool Type::can_be_container() const {
	if (auto c = dynamic_cast<const Compound_type*>(this)) {
		return c->some([&](const Type* type) {
			return type->container();
		});
	}
	return is_any() or container();
}
bool Type::can_be_callable() const {
	if (auto c = dynamic_cast<const Compound_type*>(this)) {
		return c->some([&](const Type* type) {
			return type->callable();
		});
	}
	return is_any() or callable();
}
bool Type::can_be_numeric() const {
	return is_any() or can_be_bool() or can_be_number();
}
bool Type::is_real() const { return folded == env.real or folded == env.const_real; }
bool Type::is_integer() const { return folded == env.integer or folded == env.const_integer; }
bool Type::is_long() const { return folded == env.long_ or folded == env.const_long; }
bool Type::is_mpz() const { return folded == env.mpz or folded == env.tmp_mpz or folded == env.const_mpz; }
bool Type::is_mpz_ptr() const { return folded == env.mpz_ptr or folded == env.tmp_mpz_ptr or folded == env.const_mpz_ptr; }
bool Type::is_string() const { return folded == env.string or folded == env.tmp_string or folded == env.const_string; }
bool Type::is_array() const { return is_type<Array_type>(); }
bool Type::is_fixed_array() const { return is_type<Fixed_array_type>(); }
bool Type::is_set() const { return is_type<Set_type>(); }
bool Type::is_interval() const { return folded == env.interval or folded == env.tmp_interval or folded == env.const_interval; }
bool Type::is_map() const { return is_type<Map_type>(); }
bool Type::is_function() const { return is_type<Function_type>(); }
bool Type::is_function_object() const { return is_type<Function_object_type>(); }
bool Type::is_function_pointer() const { return is_pointer() and pointed()->is_type<Function_type>(); }
bool Type::is_object() const { return folded == env.object; }
bool Type::is_never() const { return folded == env.never; }
bool Type::is_null() const { return folded == env.null; }
bool Type::is_class() const { return is_type<Class_type>(); }
bool Type::is_pointer() const { return is_type<Pointer_type>(); }
bool Type::is_struct() const { return is_type<Struct_type>(); }
bool Type::is_closure() const {
	if (auto f = dynamic_cast<const Function_type*>(folded)) {
		return f->closure();
	}
	if (auto f = dynamic_cast<const Function_object_type*>(folded)) {
		return f->closure();
	}
	return false;
}
bool Type::is_polymorphic() const {
	// TODO extends all polymorphic types from Polymorphic_type (Any_type) to improve check
	return dynamic_cast<const String_type*>(folded) != nullptr
		or dynamic_cast<const Array_type*>(folded) != nullptr
		or dynamic_cast<const Set_type*>(folded) != nullptr
		or dynamic_cast<const Map_type*>(folded) != nullptr
		or dynamic_cast<const Interval_type*>(folded) != nullptr
		or dynamic_cast<const Any_type*>(folded) != nullptr
		or dynamic_cast<const Function_object_type*>(folded) != nullptr
		or dynamic_cast<const Class_type*>(folded) != nullptr
		or dynamic_cast<const Object_type*>(folded) != nullptr
		or dynamic_cast<const Null_type*>(folded) != nullptr;
}
bool Type::is_primitive() const {
	return dynamic_cast<const Integer_type*>(folded) != nullptr
		or dynamic_cast<const Long_type*>(folded) != nullptr
		or dynamic_cast<const Real_type*>(folded) != nullptr
		or dynamic_cast<const Bool_type*>(folded) != nullptr
		or dynamic_cast<const Function_type*>(folded) != nullptr;
}
bool Type::is_void() const {
	return this == env.void_;
}
bool Type::is_template() const { return is_type<Template_type>(); }

bool Type::castable(const Type* type, bool strictCast) const {
	auto d = distance(type);
	return d >= 0 and (!strictCast or d < 100000);
}
bool Type::strictCastable(const Type* type) const {
	auto d = distance(type);
	return d >= 0 and d < 100;
}

const Type* Type::array(const Type* element) {
	if (auto e = dynamic_cast<const Meta_element_type*>(element)) return e->type;
	Environment& env = element->env;
	element = element->not_temporary();
	auto i = env.array_types.find(element);
	if (i != env.array_types.end()) return i->second.get();
	auto type = new Array_type(element);
	type->placeholder = element->placeholder;
	env.array_types.emplace(element, type);
	return type;
}

const Type* Type::const_array(const Type* element) {
	if (auto e = dynamic_cast<const Meta_element_type*>(element)) return e->type;
	Environment& env = element->env;
	element = element->not_temporary();
	auto i = env.const_array_types.find(element);
	if (i != env.const_array_types.end()) return i->second;
	auto type = array(element)->add_constant();
	env.const_array_types.insert({element, type });
	return type;
}
const Type* Type::tmp_array(const Type* element) {
	if (auto e = dynamic_cast<const Meta_element_type*>(element)) return e->type;
	auto& env = element->env;
	element = element->not_temporary();
	auto i = env.tmp_array_types.find(element);
	if (i != env.tmp_array_types.end()) return i->second;
	auto type = array(element)->add_temporary();
	env.tmp_array_types.insert({element, type });
	return type;
}

const Type* Type::fixed_array(std::vector<const Type*> elements) {
	assert(elements.size());
	// if (auto e = dynamic_cast<const Meta_element_type*>(element)) return e->type;
	Environment& env = elements.front()->env;
	for (auto& element : elements) {
		element = element->not_temporary();
	}
	auto i = env.fixed_array_types.find(elements);
	if (i != env.fixed_array_types.end()) return i->second.get();
	auto type = new Fixed_array_type(elements);
	// type->placeholder = element->placeholder;
	env.fixed_array_types.emplace(elements, type);
	return type;
}

const Type* Type::tmp_fixed_array(std::vector<const Type*> elements) {
	// assert(elements.size());
	// if (auto e = dynamic_cast<const Meta_element_type*>(element)) return e->type;
	auto& env = elements.front()->env;
	for (auto& element : elements) {
		element = element->not_temporary();
	}
	auto i = env.tmp_fixed_array_types.find(elements);
	if (i != env.tmp_fixed_array_types.end()) return i->second;
	auto type = fixed_array(elements)->add_temporary();
	env.tmp_fixed_array_types.insert({elements, type });
	return type;
}

const Type* Type::set(const Type* element) {
	auto& env = element->env;
	auto i = env.set_types.find(element);
	if (i != env.set_types.end()) return i->second.get();
	auto type = new Set_type(element);
	env.set_types.emplace(element, type);
	return type;
}
const Type* Type::const_set(const Type* element) {
	auto& env = element->env;
	auto i = env.const_set_types.find(element);
	if (i != env.const_set_types.end()) return i->second;
	auto type = set(element)->add_constant();
	env.const_set_types.insert({element, type });
	return type;
}
const Type* Type::tmp_set(const Type* element) {
	auto& env = element->env;
	auto i = env.tmp_set_types.find(element);
	if (i != env.tmp_set_types.end()) return i->second;
	auto type = set(element)->add_temporary();
	env.tmp_set_types.insert({element, type });
	return type;
}
const Type* Type::map(const Type* key, const Type* element) {
	auto& env = element->env;
	auto i = env.map_types.find({key, element});
	if (i != env.map_types.end()) return i->second.get();
	auto type = new Map_type(key, element);
	env.map_types.emplace(std::make_pair(key, element), type);
	return type;
}
const Type* Type::const_map(const Type* key, const Type* element) {
	auto& env = element->env;
	auto i = env.const_map_types.find({key, element});
	if (i != env.const_map_types.end()) return i->second;
	auto type = map(key, element)->add_constant();
	env.const_map_types.emplace(std::make_pair(key, element), type);
	return type;
}
const Type* Type::tmp_map(const Type* key, const Type* element) {
	auto& env = element->env;
	auto i = env.tmp_map_types.find({key, element});
	if (i != env.tmp_map_types.end()) return i->second;
	auto type = map(key, element)->add_temporary();
	env.tmp_map_types.insert({{key, element}, type });
	return type;
}
const Type* Type::fun(const Type* return_type, std::vector<const Type*> arguments, const Value* function) {
	auto& env = return_type->env;
	if (function == nullptr) {
		std::pair<const Type*, std::vector<const Type*>> key { return_type, arguments };
		auto i = env.function_types.find(key);
		if (i != env.function_types.end()) return i->second.get();
		auto type = new Function_type { return_type, arguments };
		env.function_types.emplace(key, type);
		return type->add_constant();
	} else {
		auto t = new Function_type(return_type, arguments, function);
		env.raw_function_types.emplace_back(t);
		return t->add_constant();
	}
}
const Type* Type::fun_object(const Type* return_type, std::vector<const Type*> arguments, const Value* function) {
	auto& env = return_type->env;
	if (function == nullptr) {
		std::pair<const Type*, std::vector<const Type*>> key { return_type, arguments };
		auto i = env.function_object_types.find(key);
		if (i != env.function_object_types.end()) return i->second.get();
		auto type = new Function_object_type(return_type, arguments);
		env.function_object_types.emplace(key, type);
		return type->add_constant();
	} else {
		auto t = new Function_object_type(return_type, arguments, false, function);
		env.raw_function_types.emplace_back(t);
		return t->add_constant();
	}
}
const Type* Type::closure(const Type* return_type, std::vector<const Type*> arguments, const Value* function) {
	auto& env = return_type->env;
	if (function == nullptr) {
		std::pair<const Type*, std::vector<const Type*>> key { return_type, arguments };
		auto i = env.closure_types.find(key);
		if (i != env.closure_types.end()) return i->second.get();
		auto type = new Function_object_type(return_type, arguments, true);
		env.closure_types.emplace(key, type);
		return type->add_constant();
	} else {
		auto t = new Function_object_type(return_type, arguments, true, function);
		env.raw_function_types.emplace_back(t);
		return t->add_constant();
	}
}
const Type* Type::structure(const std::string name, std::initializer_list<const Type*> types) {
	auto& env = (*types.begin())->env;
	auto i = env.structure_types.find(name);
	if (i != env.structure_types.end()) return i->second.get();
	auto type = new Struct_type(name, types);
	env.structure_types.emplace(name, type);
	return type;
}
const Type* Type::compound(std::initializer_list<const Type*> types) {
	return Type::compound(std::vector<const Type*>(types));
}
const Type* Type::compound(std::vector<const Type*> types) {
	if (types.size() == 1) return *types.begin();
	auto& env = types[0]->env;
	std::set<const Type*> base;
	auto folded = env.void_;
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
	auto i = env.compound_types.find(base);
	if (i != env.compound_types.end()) {
		if (temporary) {
			return i->second->add_temporary();
		}
		return i->second.get();
	}
	auto type = new Compound_type { base, folded };
	env.compound_types.emplace(base, type);
	if (temporary) {
		// std::cout << "temporary" << std::endl;
		return type->add_temporary();
	}
	return type;
}
const Type* Type::tmp_compound(std::initializer_list<const Type*> types) {
	return compound(types)->add_temporary();
}

const Type* Type::meta_add(const Type* t1, const Type* t2) {
	auto& env = t1->env;
	auto i = env.meta_add_types.find({t1, t2});
	if (i != env.meta_add_types.end()) return i->second.get();
	auto type = new Meta_add_type(t1, t2);
	env.meta_add_types.emplace(std::make_pair(t1, t2), type);
	return type;
}

const Type* Type::meta_concat(const Type* t1, const Type* t2) {
	auto& env = t1->env;
	auto i = env.meta_concat_types.find({t1, t2});
	if (i != env.meta_concat_types.end()) return i->second.get();
	auto type = new Meta_concat_type(t1, t2);
	env.meta_concat_types.emplace(std::make_pair(t1, t2), type);
	return type;
}

const Type* Type::meta_mul(const Type* t1, const Type* t2) {
	auto& env = t1->env;
	auto i = env.meta_mul_types.find({t1, t2});
	if (i != env.meta_mul_types.end()) return i->second.get();
	auto type = new Meta_mul_type(t1, t2);
	env.meta_mul_types.emplace(std::make_pair(t1, t2), type);
	return type;
}

const Type* Type::meta_base_of(const Type* type, const Type* base) {
	auto& env = type->env;
	auto i = env.meta_base_of_types.find({type, base});
	if (i != env.meta_base_of_types.end()) return i->second.get();
	auto r = new Meta_baseof_type(type, base);
	env.meta_base_of_types.emplace(std::make_pair(type, base), r);
	return r;
}

const Type* Type::meta_element(const Type* base) {
	auto& env = base->env;
	auto i = env.meta_element_types.find(base);
	if (i != env.meta_element_types.end()) return i->second.get();
	auto type = new Meta_element_type(base);
	env.meta_element_types.emplace(base, type);
	return type;
}

const Type* Type::meta_temporary(const Type* base) {
	auto& env = base->env;
	auto i = env.meta_temporary_types.find(base);
	if (i != env.meta_temporary_types.end()) return i->second.get();
	auto type = new Meta_temporary_type(base);
	env.meta_temporary_types.emplace(type, type);
	return type;
}

const Type* Type::meta_not_temporary(const Type* base) {
	auto& env = base->env;
	auto i = env.meta_not_temporary_types.find(base);
	if (i != env.meta_not_temporary_types.end()) return i->second.get();
	auto type = new Meta_not_temporary_type(base);
	env.meta_not_temporary_types.emplace(type, type);
	return type;
}

const Type* Type::meta_not_void(const Type* base) {
	auto& env = base->env;
	auto i = env.meta_not_void_types.find(base);
	if (i != env.meta_not_void_types.end()) return i->second.get();
	auto type = new Meta_not_void_type(base);
	env.meta_not_void_types.emplace(type, type);
	return type;
}

std::ostream& operator << (std::ostream& os, const Type* type) {
	if (type->constant and not type->is_function()) {
		os << BLUE_BOLD << "const:" << END_COLOR;
	}
	type->print(os);
	if (type->temporary) {
		os << BLUE_BOLD << "&&" << END_COLOR;
	} else if (type->reference) {
		os << BLUE_BOLD << "&" << END_COLOR;
	}
	return os;
}

std::ostream& operator << (std::ostream& os, const std::vector<const Type*>& types) {
	os << "[";
	for (unsigned i = 0; i < types.size(); ++i) {
		if (i > 0) os << ", ";
		os << types[i];
	}
	os << "]";
	return os;
}

#if COMPILER
std::ostream& operator << (std::ostream& os, const llvm::Type* type) {
	std::string str;
	llvm::raw_string_ostream rso(str);
	type->print(rso);
	os << rso.str();
	return os;
}
#endif

}
