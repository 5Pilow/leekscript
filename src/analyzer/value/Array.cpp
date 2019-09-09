#include "Array.hpp"
#include "../../vm/value/LSArray.hpp"
#include "../../type/Type.hpp"
#include <math.h>

namespace ls {

Array::Array() {
	type = Type::array(Type::never);
}

void Array::print(std::ostream& os, int indent, bool debug, bool condensed) const {
	os << "[";
	for (size_t i = 0; i < expressions.size(); ++i) {
		expressions[i]->print(os, indent, debug, condensed);
		if (i < expressions.size() - 1) {
			os << ", ";
		}
	}
	os << "]";
	if (debug) {
		os << " " << type;
	}
}

Location Array::location() const {
	return {opening_bracket->location.file, opening_bracket->location.start, closing_bracket->location.end};
}

void Array::pre_analyze(SemanticAnalyzer* analyzer) {
	for (size_t i = 0; i < expressions.size(); ++i) {
		const auto& ex = expressions[i];
		ex->pre_analyze(analyzer);
	}
}

void Array::analyze(SemanticAnalyzer* analyzer) {

	// std::cout << "Array::analyze " << req_type << std::endl;
	constant = true;
	throws = false;

	if (expressions.size() > 0) {

		const Type* element_type = Type::void_;
		auto homogeneous = true;

		// First analyze pass
		for (size_t i = 0; i < expressions.size(); ++i) {

			const auto& ex = expressions[i];
			ex->analyze(analyzer);
			ex->must_return_any(analyzer);

			constant &= ex->constant;
			throws |= ex->throws;
			if (element_type->is_void() and element_type != ex->type) {
				homogeneous = false;
			}
			element_type = element_type->operator * (ex->type);
		}

		// Re-analyze expressions with the supported type
		// and second computation of the array type
		const Type* new_element_type = Type::void_;
		for (size_t i = 0; i < expressions.size(); ++i) {
			const auto& ex = expressions[i];
			if (!homogeneous and ex->type->is_array()) {
				// If the array stores other arrays of different types,
				// force those arrays to store pointers. (To avoid having unknown array<int> inside arrays.
				ex->will_store(analyzer, Type::any);
			}
			if (ex->type->is_function()) {
				std::vector<const Type*> types;
				for (unsigned p = 0; p < ex->type->arguments().size(); ++p) {
					types.push_back(Type::any);
				}
				if (types.size() > 0) {
					ex->will_take(analyzer, types, 1);
				}
			}
			new_element_type = new_element_type-> operator + (ex->type);
		}
		type = Type::array(new_element_type->not_temporary());
	}
	type = type->add_temporary();
	// std::cout << "Array type : " << type << " " << type->element()->fold() << std::endl;
}

void Array::elements_will_take(SemanticAnalyzer* analyzer, const std::vector<const Type*>& arg_types, int level) {

	// std::cout << "Array::elements_will_take " << arg_types << " at " << level << std::endl;

	for (size_t i = 0; i < expressions.size(); ++i) {
		const auto& arr = dynamic_cast<Array*>(expressions[i].get());
		if (arr != nullptr && level > 0) {
			arr->elements_will_take(analyzer, arg_types, level - 1);
		} else {
			expressions[i]->will_take(analyzer, arg_types, 1);
		}
	}
	// Computation of the new array type
	const Type* element_type = Type::void_;
	for (unsigned i = 0; i < expressions.size(); ++i) {
		const auto& ex = expressions[i];
		if (i == 0) {
			element_type = ex->type;
		} else {
			element_type = element_type->operator * (ex->type);
		}
	}
	this->type = Type::array(element_type);
	// std::cout << "Array::elements_will_take type after " << this->type << std::endl;
}

bool Array::will_store(SemanticAnalyzer* analyzer, const Type* type) {

	// std::cout << "Array::will_store " << this->type << " " << type << std::endl;

	auto added_type = type;
	if (added_type->is_array() or added_type->is_set()) {
		added_type = added_type->element();
	}
	auto current_type = this->type->element();
	if (expressions.size() == 0) {
		this->type = Type::array(added_type);
	} else {
		this->type = Type::array(current_type->operator + (added_type));
	}
	
	return false;
}

bool Array::elements_will_store(SemanticAnalyzer* analyzer, const Type* type, int level) {
	for (auto& element : expressions) {
		element->will_store(analyzer, type);
	}
	// Computation of the new array type
	const Type* element_type = Type::void_;
	for (unsigned i = 0; i < expressions.size(); ++i) {
		const auto& ex = expressions[i];
		if (i == 0) {
			element_type = ex->type;
		} else {
			element_type = element_type->operator * (ex->type);
		}
	}
	this->type = Type::array(element_type);
	return false;
}

Compiler::value Array::compile(Compiler& c) const {
	std::vector<Compiler::value> elements;
	for (const auto& val : expressions) {
		auto v = val->compile(c);
		val->compile_end(c);
		elements.push_back(v);
	}
	return c.new_array(type->element(), elements);
}

std::unique_ptr<Value> Array::clone() const {
	auto array = std::make_unique<Array>();
	array->opening_bracket = opening_bracket;
	array->closing_bracket = closing_bracket;
	for (const auto& ex : expressions) {
		array->expressions.push_back(ex->clone());
	}
	return array;
}

}
