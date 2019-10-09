#include "Function_object_type.hpp"
#include "Function_type.hpp"
#include "../colors.h"
#include "Type.hpp"
#include "Placeholder_type.hpp"
#include "Any_type.hpp"
#include "Struct_type.hpp"
#include "../environment/Environment.hpp"

namespace ls {

Function_object_type::Function_object_type(const Type* ret, const std::vector<const Type*>& args, bool closure, const Value* function) : Pointer_type(Type::structure(closure ? "closure" : "function", {
	ret->env.integer, // ?
	ret->env.integer, // ?
	ret->env.integer, // ?
	ret->env.integer, // refs
	ret->env.boolean, // native
	ret->env.long_->pointer() // pointer to the function
}), true), _return_type(ret), _arguments(args), _closure(closure), _function(function) {}

bool Function_object_type::operator == (const Type* type) const {
	if (auto fun = dynamic_cast<const Function_object_type*>(type)) {
		return _return_type == fun->_return_type && _arguments == fun->_arguments;
	}
	return false;
}
int Function_object_type::distance(const Type* type) const {
	if (not temporary and type->temporary) return -1;
	if (dynamic_cast<const Any_type*>(type->folded)) { return 1; }
	if (auto fun = dynamic_cast<const Function_object_type*>(type->folded)) {
		if (_arguments.size() > fun->_arguments.size()) return -1;
		int d = 0;
		for (size_t i = 0; i < _arguments.size(); ++i) {
			d += _arguments.at(i)->distance(fun->_arguments.at(i)->not_temporary());
		}
		int d2 = (_closure != fun->_closure) ? 1 : 0;
		if (d == 0) {
			return d2 + _return_type->distance(fun->_return_type);
		} else {
			return d + d2;
		}
	}
	if (auto fun = dynamic_cast<const Function_type*>(type->folded)) {
		if (_arguments.size() > fun->arguments().size()) return -1;
		int d = 0;
		for (size_t i = 0; i < _arguments.size(); ++i) {
			d += _arguments.at(i)->distance(fun->arguments().at(i)->not_temporary());
		}
		int d2 = (_closure) ? 1 : 0;
		if (d == 0) {
			return d2 + _return_type->distance(fun->return_type());
		} else {
			return d + d2;
		}
	}
	return -1;
}
const Type* Function_object_type::return_type() const {
	return _return_type;
}
const std::vector<const Type*>& Function_object_type::arguments() const {
	return _arguments;
}
const Type* Function_object_type::argument(size_t i) const {
	// assert(i < _arguments.size() && "Wrong argument() index");
	if (i < _arguments.size()) {
		return _arguments[i];
	}
	return env.any;
}
std::string Function_object_type::class_name() const {
	return "Function";
}
Json Function_object_type::json() const {
	Json args;
	for (const auto& arg : _arguments) {
		args.push_back(arg->json());
	}
	return {
		{ "name", "function" },
		{ "args", args },
		{ "return", _return_type->json() }
	};
}
std::ostream& Function_object_type::print(std::ostream& os) const {
	os << BLUE_BOLD << (_closure ? "closure(" : "fun_obj(") << END_COLOR;
	for (unsigned t = 0; t < _arguments.size(); ++t) {
		if (t > 0) os << ", ";
		os << _arguments[t];
	}
	os << BLUE_BOLD << ") => " << _return_type;
	return os;
}
Type* Function_object_type::clone() const {
	return new Function_object_type { _return_type, _arguments, _closure, _function };
}

}