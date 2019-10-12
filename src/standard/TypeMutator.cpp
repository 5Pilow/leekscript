#include "TypeMutator.hpp"
#include "../analyzer/value/LeftValue.hpp"
#include "../analyzer/value/VariableValue.hpp"
#include "../analyzer/value/ArrayAccess.hpp"
#include "../analyzer/semantic/Variable.hpp"
#include "../type/Type.hpp"
#include "Module.hpp"
#include "../analyzer/value/Phi.hpp"

namespace ls {

void ConvertMutator::apply(SemanticAnalyzer* analyzer, std::vector<Value*> values, const Type* return_type) const {
	// std::cout << "ConvertMutator " << values[0]->type << " " << return_type << std::endl;
	if (auto vv = dynamic_cast<VariableValue*>(values[0])) {
		// std::cout << "Convert mutator " << (void*) vv->var->parent << " => " << vv->var << " " << vv->var->type << " " << return_type << std::endl;
		vv->previous_type = vv->type;
		vv->type = return_type->not_temporary();
		vv->var->type = vv->type;
	} else if (auto aa = dynamic_cast<ArrayAccess*>(values[0])) {
		if (auto vv = dynamic_cast<VariableValue*>(aa->array.get())) {
			if (vv->type->is_array()) {
				vv->type = Type::array(return_type->not_temporary());
			} else if (vv->type->is_map()) {
				vv->type = Type::map(vv->type->key(), return_type->not_temporary());
			}
			vv->var->type = vv->type;
		}
	}
}

#if COMPILER
int ConvertMutator::compile(Compiler& c, CallableVersion* callable, std::vector<Value*> values) const {
	// std::cout << "ConvertMutator" << std::endl;
	if (auto vv = dynamic_cast<VariableValue*>(values[0])) {
		if (vv->var->scope == VarScope::INTERNAL) return 0;
		if (store_array_size) {
			callable->extra_arg = c.insn_array_size(vv->var->parent->get_value(c));
		}
		for (const auto& phi : vv->var->phis) {
			if (phi->variable2 == vv->var) {
				// c.insn_delete_temporary(phi->value1);
			}
		}
		if (vv->var->scope == VarScope::CAPTURE) {
			vv->var->entry = vv->var->parent->entry;
			return 0;
		}
		// std::cout << "ConvertMutator " << vv->var << " " << vv->previous_type << " => " << vv->type << std::endl;
		if (vv->previous_type == vv->type) {
			// if (not vv->var->val.v) {
				vv->var->entry = vv->var->parent->entry;
			// }
		} else {
			auto previous_value = c.insn_load(vv->var->parent->entry);
			auto converted = c.insn_convert(previous_value, vv->var->type);
			if (previous_value.v == converted.v) {
				vv->var->entry = vv->var->parent->entry;
			} else {
				// std::cout << "ConvertMutator new entry" << std::endl;
				vv->var->create_entry(c);
				vv->var->store_value(c, c.insn_move_inc(converted));
				c.insn_delete_variable(vv->var->parent->entry);
			}
		}
	} else if (auto aa = dynamic_cast<ArrayAccess*>(values[0])) {
		if (auto vv = dynamic_cast<VariableValue*>(aa->array.get())) {
			auto previous_value = c.insn_load(vv->var->parent->entry);
			auto converted = c.insn_convert(previous_value, vv->var->type);
			if (previous_value.v == converted.v) {
				vv->var->entry = vv->var->parent->entry;
			} else {
				vv->var->create_entry(c);
				vv->var->store_value(c, c.insn_move_inc(converted));
				c.insn_delete_variable(vv->var->parent->entry);
			}
		} else if (auto aa2 = dynamic_cast<ArrayAccess*>(aa->array.get())) {
			if (auto vv2 = dynamic_cast<VariableValue*>(aa2->array.get())) {
				auto previous_value = c.insn_load(vv2->var->parent->entry);
				auto converted = c.insn_convert(previous_value, vv2->var->type);
				if (previous_value.v == converted.v) {
					vv2->var->entry = vv2->var->parent->entry;
				} else {
					vv2->var->create_entry(c);
					vv2->var->store_value(c, c.insn_move_inc(converted));
					c.insn_delete_variable(vv2->var->parent->entry);
				}
			}
		}
	}
	return 0;
}
#endif

void ChangeValueMutator::apply(SemanticAnalyzer* analyzer, std::vector<Value*> values, const Type* return_type) const {
	// std::cout << "Change value mutator " << values[0]->type << " => " << return_type << std::endl;
	if (auto vv = dynamic_cast<VariableValue*>(values[0])) {
		// std::cout << "Change mutator " << vv->var->parent << " " << vv->var->parent->type << " => " << vv->var << " " << vv->var->type << " " << return_type << std::endl;
		vv->previous_type = vv->type;
		vv->type = return_type->not_temporary()->not_constant();
		vv->var->type = vv->type;
		// std::cout << "variable type " << vv->var << " " << vv->var->type << std::endl;
	} else if (auto aa = dynamic_cast<ArrayAccess*>(values[0])) {
		if (aa->key) {
			if (auto vv = dynamic_cast<VariableValue*>(aa->array.get())) {
				if (vv->type->is_array()) {
					vv->type = Type::array(return_type->not_temporary());
				} else if (vv->type->is_map()) {
					vv->type = Type::map(aa->key->type->not_temporary(), return_type->not_temporary());
				}
				vv->var->type = vv->type;
			} else if (auto aa2 = dynamic_cast<ArrayAccess*>(aa->array.get())) {
				if (auto vv2 = dynamic_cast<VariableValue*>(aa2->array.get())) {
					// vv->type = Type::array(Type::array(return_type->not_temporary()));
					// vv->var->type = vv->type;
				}
			}
		}
	}
}

#if COMPILER
int ChangeValueMutator::compile(Compiler& c, CallableVersion* callable, std::vector<Value*> values) const {
	// std::cout << "ChangeValueMutator compile" << std::endl;
	if (auto vv = dynamic_cast<VariableValue*>(values[0])) {
		// std::cout << "ChangeValueMutator " << vv->var << " ||| " << vv->previous_type << " == " << vv->type << std::endl;
		// if (vv->var->phi and vv->var->phi->variable2 == vv->var) {
			// c.insn_delete_temporary(vv->var->phi->value1);
		// }
		if (vv->previous_type == vv->type) {
			if (vv->var->entry.v) {
				// std::cout << "ChangeValueMutator no change " << vv->var << std::endl;
			} else {
				// std::cout << "ChangeValueMutator take parent " << vv->var << std::endl;
				vv->var->entry = vv->var->parent->entry;
			}
			for (const auto& phi : vv->var->phis) {
				if (phi->variable2 == vv->var) {
					// std::cout << "type mutator load value" << std::endl;
					if (vv->var->type->is_mpz_ptr()) {
						phi->value2 = vv->var->entry;
					} else {
						phi->value2 = c.insn_load(vv->var->entry);
					}
				}
			}
			return 0;
		} else {
			// std::cout << "ChangeValueMutator new_entry " << vv->var << std::endl;
			vv->var->create_entry(c);
			// std::cout << "delete parent : " << vv->var->parent << std::endl;
			c.insn_delete_variable(vv->var->parent->entry);
			return Module::EMPTY_VARIABLE;
		}
	} else if (auto aa = dynamic_cast<ArrayAccess*>(values[0])) {
		// std::cout << "ChangeValue::compile ArrayAccess" << std::endl;
		if (auto vv = dynamic_cast<VariableValue*>(aa->array.get())) {
			// vv->var->val = vv->var->parent->val;
			auto previous_value = c.insn_load(vv->var->parent->entry);
			auto converted = c.insn_convert(previous_value, vv->var->type);
			if (previous_value.v == converted.v) {
				vv->var->entry = vv->var->parent->entry;
			} else {
				vv->var->create_entry(c);
				vv->var->store_value(c, c.insn_move_inc(converted));
				c.insn_delete_variable(vv->var->parent->entry);
			}
		} else if (auto aa2 = dynamic_cast<ArrayAccess*>(aa->array.get())) {
			if (auto vv2 = dynamic_cast<VariableValue*>(aa2->array.get())) {
				vv2->var->entry = vv2->var->parent->entry;
			}
		}
	}
	return 0;
}
#endif

void WillTakeMutator::apply(SemanticAnalyzer* analyzer, std::vector<Value*> values, const Type* return_type) const {
	auto& env = analyzer->env;
	values[1]->will_take(analyzer, { env.any }, 1);
	// std::vector<Type> args;
	// for (const auto& extractor : types) {
	// 	args.push_back(extractor->extract(analyzer, values));
	// }
}

const Type* TypeExtractor::extract(SemanticAnalyzer* analyzer, std::vector<Value*> values) const {
	return analyzer->env.void_;
}

const Type* ElementExtractor::extract(SemanticAnalyzer* analyzer, std::vector<Value*> values) const {
	return extractor->extract(analyzer, values)->element();
}

const Type* ArgumentExtractor::extract(SemanticAnalyzer* analyzer, std::vector<Value*> values) const {
	return values.at(index)->type;
}

}
