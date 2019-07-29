#include "LSFunction.hpp"
#include "LSNull.hpp"
#include "LSClass.hpp"
#include "LSNumber.hpp"
#include "LSArray.hpp"
#include "../../vm/VM.hpp"

namespace ls {

LSFunction* LSFunction::constructor(VM* vm, void* f) {
	auto fun = new LSFunction(f);
	vm->function_created.push_back(fun);
	return fun;
}

LSFunction::LSFunction(void* function) : LSValue(FUNCTION, 1, true) {
	this->function = function;
}

LSFunction::~LSFunction() {}

bool LSFunction::closure() const {
	return false;
}

/*
 * LSValue methods
 */
bool LSFunction::to_bool() const {
	return true;
}

bool LSFunction::ls_not() const {
	return false;
}

bool LSFunction::eq(const LSValue* v) const {
	if (auto f = dynamic_cast<const LSFunction*>(v)) {
		return function == f->function;
	}
	return false;
}

bool LSFunction::lt(const LSValue* v) const {
	if (auto f = dynamic_cast<const LSFunction*>(v)) {
		return function < f->function;
	}
	return LSValue::lt(v);
}

LSValue* LSFunction::attr(VM* vm, const std::string& key) const {
	if (key == "args") {
		LSArray<LSValue*>* args_list = new LSArray<LSValue*>();
		for (const auto& arg : args) {
			args_list->push_back(arg);
		}
		return args_list;
	}
	if (key == "return") {
		return return_type;
	}
	return LSValue::attr(vm, key);
}

LSValue* LSFunction::clone() const {
	return (LSValue*) this;
}

std::ostream& LSFunction::dump(std::ostream& os, int) const {
	os << "<function>";
	return os;
}

std::string LSFunction::json() const {
	return ""; // don't output function in json
}

LSValue* LSFunction::getClass(VM* vm) const {
	return vm->env.function_class.get();
}

}
