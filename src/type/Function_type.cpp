#include "Function_type.hpp"
#include "Function_object_type.hpp"
#include "../colors.h"
#include "Type.hpp"
#include "Placeholder_type.hpp"
#include "Any_type.hpp"
#include "Struct_type.hpp"

namespace ls {

Function_type::Function_type(const Type* ret, const std::vector<const Type*>& args, const Value* function) : Type(true), _return_type(ret), _arguments(args), _function(function) {}

bool Function_type::operator == (const Type* type) const {
	if (auto fun = dynamic_cast<const Function_type*>(type)) {
		return _return_type == fun->_return_type && _arguments == fun->_arguments;
	}
	return false;
}
int Function_type::distance(const Type* type) const {
	if (not temporary and type->temporary) return -1;
	if (dynamic_cast<const Any_type*>(type->folded)) { return 1; }
	if (auto fun = dynamic_cast<const Function_type*>(type->folded)) {
		if (_arguments.size() > fun->_arguments.size()) return -1;
		int d = 0;
		for (size_t i = 0; i < _arguments.size(); ++i) {
			d += _arguments.at(i)->distance(fun->_arguments.at(i)->not_temporary());
		}
		if (d == 0) {
			return _return_type->distance(fun->_return_type);
		} else {
			return d;
		}
	}
	if (auto fun = dynamic_cast<const Function_object_type*>(type->folded)) {
		if (_arguments.size() > fun->arguments().size()) return -1;
		int d = 0;
		for (size_t i = 0; i < _arguments.size(); ++i) {
			d += _arguments.at(i)->distance(fun->arguments().at(i)->not_temporary());
		}
		int d2 = fun->closure() ? 1 : 0;
		if (d == 0) {
			return d2 + _return_type->distance(fun->return_type());
		} else {
			return d;
		}
	}
	return -1;
}
const Type* Function_type::return_type() const {
	return _return_type;
}
const std::vector<const Type*>& Function_type::arguments() const {
	return _arguments;
}
const Type* Function_type::argument(size_t i) const {
	// assert(i < _arguments.size() && "Wrong argument() index");
	if (i < _arguments.size()) {
		return _arguments[i];
	}
	return Type::any;
}
#if COMPILER
llvm::Type* Function_type::llvm(Compiler& c) const {
	if (llvm_type) return llvm_type;
	std::vector<llvm::Type*> llvm_types;
	for (const auto& a : _arguments) {
		llvm_types.push_back(a->llvm(c));
	}
	((Function_type*) this)->llvm_type = llvm::FunctionType::get(_return_type->llvm(c), llvm_types, false);
	return llvm_type;
}
#endif
std::string Function_type::class_name() const {
	return "Function";
}
std::ostream& Function_type::print(std::ostream& os) const {
	os << BLUE_BOLD << "fun(" << END_COLOR;
	for (unsigned t = 0; t < _arguments.size(); ++t) {
		if (t > 0) os << ", ";
		os << _arguments[t];
	}
	os << BLUE_BOLD << ") => " << _return_type;
	return os;
}
Type* Function_type::clone() const {
	return new Function_type { _return_type, _arguments, _function };
}

}