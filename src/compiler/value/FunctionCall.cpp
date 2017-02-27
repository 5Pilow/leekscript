#include "FunctionCall.hpp"

#include <jit/jit-common.h>
#include <jit/jit-insn.h>
#include <jit/jit-type.h>
#include <jit/jit-value.h>
#include <sstream>
#include <string>

#include "../../vm/Module.hpp"
#include "../../vm/Program.hpp"
#include "../../vm/Type.hpp"
#include "../../vm/value/LSArray.hpp"
#include "../../vm/value/LSClass.hpp"
#include "../../vm/value/LSNumber.hpp"
#include "../../vm/value/LSObject.hpp"
#include "../../vm/value/LSString.hpp"
#include "../../vm/VM.hpp"
#include "../Compiler.hpp"
#include "../lexical/Token.hpp"
#include "../semantic/SemanticAnalyser.hpp"
#include "../semantic/SemanticError.hpp"
#include "ObjectAccess.hpp"
#include "VariableValue.hpp"

using namespace std;

namespace ls {

FunctionCall::FunctionCall(Token* t) {
	token = t;
	function = nullptr;
	type = Type::UNKNOWN;
	std_func = nullptr;
	this_ptr = nullptr;
}

FunctionCall::~FunctionCall() {
	delete function;
	for (const auto& arg : arguments) {
		delete arg;
	}
}

void FunctionCall::print(std::ostream& os, int indent, bool debug) const {

	function->print(os, indent, debug);
	os << "(";
	for (unsigned i = 0; i < arguments.size(); ++i) {
		arguments.at(i)->print(os, indent, debug);
		if (i < arguments.size() - 1) {
			os << ", ";
		}
	}
	os << ")";
	if (debug) {
		os << " " << type;
	}
}

void FunctionCall::analyse(SemanticAnalyser* analyser, const Type& req_type) {

//	cout << "function call analyse : " << req_type << endl;

	constant = false;

	function->analyse(analyser, Type::UNKNOWN);

	if (function->type.raw_type != RawType::UNKNOWN and
		function->type.raw_type != RawType::FUNCTION and
		function->type.raw_type != RawType::CLASS) {
		analyser->add_error({SemanticError::Type::CANNOT_CALL_VALUE,
			function->line(), {function->to_string()}});
	}

	int a = 0;
	for (Value* arg : arguments) {
//		arg->analyse(analyser, function->type.getArgumentType(a++));
		arg->analyse(analyser, Type::UNKNOWN);
	}

	// Standard library constructors
	VariableValue* vv = dynamic_cast<VariableValue*>(function);
	if (vv != nullptr) {
		if (vv->name == "Number") type = Type::INTEGER;
		if (vv->name == "Boolean") type = Type::BOOLEAN;
		if (vv->name == "String") type = Type::STRING;
		if (vv->name == "Array") type = Type::PTR_ARRAY;
		if (vv->name == "Object") type = Type::OBJECT;
	}

	// Detect standard library functions
	ObjectAccess* oa = dynamic_cast<ObjectAccess*>(function);
	if (oa != nullptr) {

		string field_name = oa->field->content;
		Type object_type = oa->object->type;

		vector<Type> arg_types;
		for (auto arg : arguments) {
			arg_types.push_back(arg->type);
		}
		std::string args_string = "";
		for (unsigned i = 0; i < arg_types.size(); ++i) {
			std::ostringstream oss;
			oss << arg_types[i];
			if (i > 0) args_string += ", ";
			args_string += oss.str();
		}

		if (object_type.raw_type == RawType::CLASS) { // String.size("salut")

			string clazz = ((VariableValue*) oa->object)->name;
			LSClass* object_class = (LSClass*) analyser->vm->system_vars[clazz];
			StaticMethod* sm = object_class->getStaticMethod(oa->field->content, arg_types);

			if (sm != nullptr) {
				std_func = sm->addr;
				function->type = sm->type;
				is_native_method = sm->native;
			} else {
				LSClass* value_class = (LSClass*) analyser->vm->system_vars["Value"];
				Method* m = value_class->getMethod(oa->field->content, object_type, arg_types);

				if (m != nullptr) {
					this_ptr = oa->object;
					this_ptr->analyse(analyser, m->obj_type);
					std_func = m->addr;
					function->type = m->type;
					is_native_method = m->native;
				} else {
					analyser->add_error({SemanticError::Type::STATIC_METHOD_NOT_FOUND, oa->field->line, {clazz + "::" + oa->field->content + "(" + args_string + ")"}});
				}
			}
		} else {  // "salut".size()

			Method* m = nullptr;
			if (object_type.raw_type != RawType::UNKNOWN) {
				LSClass* object_class = (LSClass*) analyser->vm->system_vars[object_type.clazz];
				m = object_class->getMethod(oa->field->content, object_type, arg_types);
			}
			if (m == nullptr) {
				LSClass* value_class = (LSClass*) analyser->vm->system_vars["Value"];
				m = value_class->getMethod(oa->field->content, object_type, arg_types);
			}
			if (m != nullptr) {
				this_ptr = oa->object;
				this_ptr->analyse(analyser, m->obj_type);
				std_func = m->addr;
				function->type = m->type;
				is_native_method = m->native;
			} else {
				if (object_type.raw_type != RawType::UNKNOWN) {
					std::ostringstream obj_type_ss;
					obj_type_ss << object_type;
					analyser->add_error({SemanticError::Type::METHOD_NOT_FOUND, oa->field->line, {obj_type_ss.str() + "." + oa->field->content + "(" + args_string + ")"}});
				} else {
					is_unknown_method = true;
					object = oa->object;
				}
			}
		}
	}

	vv = dynamic_cast<VariableValue*>(function);
	if (vv != nullptr) {
		string name = vv->name;
		if (name == "+" or name == "-" or name == "*" or name == "/" or name == "**" or name == "%") {
			bool isByValue = true;
			Type effectiveType;
			for (Value* arg : arguments) {
				arg->analyse(analyser, Type::UNKNOWN);
				effectiveType = arg->type;
				if (arg->type.nature != Nature::VALUE) {
					isByValue = false;
				}
			}
			if (isByValue) {
				function->type.setArgumentType(0, effectiveType);
				function->type.setArgumentType(1, effectiveType);
				function->type.setReturnType(effectiveType);
			}
			type = function->type.getReturnType();
		}
	}

	// Check argument count
	if (function->type.raw_type == RawType::FUNCTION && function->type.getArgumentTypes().size() != arguments.size()) {
		analyser->add_error({SemanticError::Type::WRONG_ARGUMENT_COUNT,	function->line(), {
			function->to_string(),
			std::to_string(function->type.getArgumentTypes().size()),
			std::to_string(arguments.size())
		}});
		type = Type::UNKNOWN;
		return;
	}

	vector<Type> arg_types;
	for (auto arg : arguments) {
		arg_types.push_back(arg->type);
	}

	a = 0;
	for (Value* arg : arguments) {
		arg->analyse(analyser, function->type.getArgumentType(a));
		if (function->type.getArgumentType(a).raw_type == RawType::FUNCTION) {
			arg->will_take(analyser, function->type.getArgumentType(a).arguments_types, 1);
		}
		a++;
	}


	function->will_take(analyser, arg_types, 1);

	// The function is a variable
	if (vv and vv->var and vv->var->value) {

		//vv->var->will_take(analyser, arg_types, 1);

		Type ret_type = vv->var->value->type.getReturnType();
		if (ret_type.raw_type != RawType::UNKNOWN) {
			type = ret_type;
		}
	} else {
		Type ret_type = function->type.getReturnType();
		if (ret_type.nature != Nature::UNKNOWN) {
			type = ret_type;
		}
		/*
		if (ret_type.raw_type != RawType::UNKNOWN) {
			type = ret_type;
		} else {
			// TODO : to be able to remove temporary variable we must know the nature
//			type = Type::POINTER; // When the function is unknown, the return type is a pointer
		}
		*/
	}

	a = 0;
	for (Value* arg : arguments) {
		Type t = function->type.getArgumentType(a);
		if (t == Type::UNKNOWN) t = Type::POINTER;
		arg->analyse(analyser, t);
		a++;
	}

	return_type = function->type.getReturnType();

	if (req_type.nature == Nature::POINTER) {
		type.nature = req_type.nature;
	}

//	cout << "Function call function type : " << type << endl;
}

bool FunctionCall::will_take(SemanticAnalyser* analyser, const std::vector<Type>& args, int level) {
	if (auto vv = dynamic_cast<VariableValue*>(function)) {
		vv->will_take(analyser, args, level + 1);
	}
	return false;
}

Compiler::value FunctionCall::compile(Compiler& c) const {

	/** Standard library constructors : Array(), Number() */
	VariableValue* vv = dynamic_cast<VariableValue*>(function);
	if (vv != nullptr) {
		if (vv->name == "Boolean") {
			jit_value_t n = jit_value_create_nint_constant(c.F, LS_INTEGER, 0);
			if (type.nature == Nature::POINTER) {
				return {VM::value_to_pointer(c.F, n, Type::BOOLEAN), type};
			}
			return {n, type};
		}
		if (vv->name == "Number") {
			jit_value_t n = LS_CREATE_INTEGER(c.F, 0);
			if (type.nature == Nature::POINTER) {
				return {VM::value_to_pointer(c.F, n, Type::INTEGER), type};
			}
			return {n, type};
		}
		if (vv->name == "String") {
			if (arguments.size() > 0) {
				return arguments[0]->compile(c);
			}
			return {c.new_pointer(new LSString("")).v, type};
		}
		if (vv->name == "Array") {
			return {c.new_pointer(new LSArray<LSValue*>()).v, type};
		}
		if (vv->name == "Object") {
			return {c.new_pointer(new LSObject()).v, type};
		}
	}

	/** Standard function call on object : "hello".size() */
	if (this_ptr != nullptr) {

		vector<Compiler::value> args = { this_ptr->compile(c) };
		for (unsigned i = 0; i < arguments.size(); ++i) {
			args.push_back(arguments[i]->compile(c));
		}

		Compiler::value res;
		if (is_native_method) {
			auto fun = (void*) std_func;
			res = c.insn_call(type, args, fun);
		} else {
			auto fun = (Compiler::value (*)(Compiler&, vector<Compiler::value>)) std_func;
			res = fun(c, args);
		}

		if (return_type.nature == Nature::VALUE and type.nature == Nature::POINTER) {
			return {VM::value_to_pointer(c.F, res.v, type), type};
		}
		return res;
	}

	/** Static standard function call : Number.char(65) */
	if (std_func != nullptr) {

		vector<Compiler::value> args;
		for (unsigned i = 0; i < arguments.size(); ++i) {
			args.push_back(arguments[i]->compile(c));
		}

		Compiler::value res;
		if (is_native_method) {
			auto fun = (void*) std_func;
			res = c.insn_call(type, args, fun);
		} else {
			auto fun = (Compiler::value (*)(Compiler&, vector<Compiler::value>)) std_func;
			res = fun(c, args);
		}

		if (return_type.nature == Nature::VALUE and type.nature == Nature::POINTER) {
			return {VM::value_to_pointer(c.F, res.v, type), type};
		}
		return res;
	}

	/** Operator functions : +(1, 2) */
	VariableValue* f = dynamic_cast<VariableValue*>(function);

	if (f != nullptr) {
		if (function->type.getArgumentType(0).nature == Nature::VALUE
			and function->type.getArgumentType(1).nature == Nature::VALUE) {

			jit_value_t (*jit_func)(jit_function_t, jit_value_t, jit_value_t) = nullptr;
			if (f->name == "+") {
				jit_func = &jit_insn_add;
			} else if (f->name == "-") {
				jit_func = &jit_insn_sub;
			} else if (f->name == "*" or f->name == "×") {
				jit_func = &jit_insn_mul;
			} else if (f->name == "/" or f->name == "÷") {
				jit_func = &jit_insn_div;
			} else if (f->name == "**") {
				jit_func = &jit_insn_pow;
			} else if (f->name == "%") {
				jit_func = &jit_insn_rem;
			}
			if (jit_func != nullptr) {
				auto v0 = arguments[0]->compile(c);
				auto v1 = arguments[1]->compile(c);
				jit_value_t ret = jit_func(c.F, v0.v, v1.v);

				if (type.nature == Nature::POINTER) {
					return {VM::value_to_pointer(c.F, ret, type), type};
				}
				return {ret, type};
			}
		}
	}

	/** Default function : f(12) */
	jit_value_t fun;
	auto ls_fun_addr = c.new_pointer(nullptr);

	if (function->type.nature == Nature::POINTER) {
		ls_fun_addr = function->compile(c);
		fun = jit_insn_load_relative(c.F, ls_fun_addr.v, 24, LS_POINTER);
	} else {
		fun = function->compile(c).v;
	}

	/** Arguments */
	int offset = 1;

	int arg_count = arguments.size() + offset;
	vector<jit_value_t> args;
	vector<jit_type_t> args_types;

	if (is_unknown_method) {
		// add 'this' object as first argument
		args.push_back(c.insn_load(((LeftValue*) object)->compile_l(c)).v);
		args_types.push_back(VM::get_jit_type(object->type));
	} else {
		// Function pointer as first argument
		args.push_back(ls_fun_addr.v);
		args_types.push_back(LS_POINTER);
	}

	for (int i = 0; i < arg_count - offset; ++i) {

		args.push_back(arguments[i]->compile(c).v);
		args_types.push_back(VM::get_jit_type(function->type.getArgumentType(i)));

		if (function->type.getArgumentType(i).must_manage_memory()) {
			args[offset + i] = c.insn_move_inc({args[offset + i], function->type.getArgumentType(i)}).v;
		}
		if (function->type.getArgumentType(i) == Type::GMP_INT &&
			arguments[i]->type != Type::GMP_INT_TMP) {
			args[offset + i] = c.insn_clone_mpz({args[offset + i], Type::GMP_INT}).v;
		}
	}

	jit_type_t jit_return_type = VM::get_jit_type(type);

	jit_type_t sig = jit_type_create_signature(jit_abi_cdecl, jit_return_type, args_types.data(), args_types.size(), 1);
	jit_value_t ret = jit_insn_call_indirect(c.F, fun, sig, args.data(), arg_count, 0);
	jit_type_free(sig);

	// Destroy temporary arguments
	for (int i = 0; i < arg_count - offset; ++i) {
		if (function->type.getArgumentType(i).must_manage_memory()) {
			c.insn_delete({args[offset + i], Type::POINTER});
		}
		if (function->type.getArgumentType(i) == Type::GMP_INT ||
			function->type.getArgumentType(i) == Type::GMP_INT_TMP) {
			c.insn_delete_mpz({args[offset + i], Type::GMP_INT});
		}
	}
	// Delete temporary function
	//if (function->type.must_manage_memory()) {
		VM::delete_temporary(c.F, fun);
	//}

	// Custom function call : 1 op
	c.inc_ops(1);

	if (return_type.nature != Nature::POINTER and type.nature == Nature::POINTER) {
		return {VM::value_to_pointer(c.F, ret, type), type};
	}
	return {ret, type};
}

}
