#include "FunctionVersion.hpp"
#include "../../type/Type.hpp"
#include "../value/Function.hpp"
#include "../Context.hpp"
#include "../resolver/File.hpp"
#include "../semantic/SemanticAnalyzer.hpp"
#include "../Program.hpp"
#include "../../colors.h"
#include "../../type/Placeholder_type.hpp"
#include "../../type/Compound_type.hpp"
#include "../../type/Function_type.hpp"
#include "Variable.hpp"
#include "../value/Phi.hpp"
#if COMPILER
#include "../../vm/VM.hpp"
#include "../../compiler/Compiler.hpp"
#endif

namespace ls {

FunctionVersion::FunctionVersion(Environment& env, std::unique_ptr<Block> body) : body(std::move(body)), type(env.void_)
#if COMPILER
, fun(env), value(env)
#endif
{}

void FunctionVersion::print(std::ostream& os, int indent, PrintOptions options) const {
	if (parent->arguments.size() != 1) {
		os << "(";
	}
	for (unsigned i = 0; i < parent->arguments.size(); ++i) {
		if (i > 0) os << ", ";
		if (options.debug and initial_arguments.find(parent->arguments.at(i)->content) != initial_arguments.end()) {
			os << initial_arguments.at(parent->arguments.at(i)->content).get();
		} else {
			os << parent->arguments.at(i)->content;
		}
		if (options.debug)
			os << " " << this->type->arguments().at(i);

		if (parent->defaultValues.at(i) != nullptr) {
			os << " = ";
			parent->defaultValues.at(i)->print(os);
		}
	}
	if (parent->arguments.size() != 1) {
		os << ")";
	}
	if (options.debug) {
		if (placeholder_type) {
			os << " " << placeholder_type;
		}
		if (recursive) {
			os << BLUE_BOLD << " recursive" << END_COLOR;
		}
		if (this->body->throws) {
			os << BLUE_BOLD << " throws" << END_COLOR;
		}
	}
	os << " => ";
	body->print(os, indent, options);

	if (options.debug) {
		// os << " [" << parent->versions.size() << " versions, " << std::boolalpha << parent->has_version << "]";
		// os << "<";
		// int i = 0;
		// for (const auto& v : parent->versions) {
		// 	if (i > 0) os << ", ";
		// 	if (v.second == this) os << "$";
		// 	os << v.first << " => " << v.second->type->return_type();
		// 	i++;
		// }
		// os << ">";
		os << " args: [";
		int i = 0;
		for (const auto& argument : arguments) {
			if (i++ > 0) os << ", ";
			os << argument.second;
		}
		os << "]";
		if (type->return_type() != body->type) {
			os << " : " << type->return_type();
		}
	}
}

const Type* FunctionVersion::getReturnType(Environment& env) {
	if (type->is_void()) {
		if (!placeholder_type) {
			placeholder_type = env.generate_new_placeholder_type();
		}
		return placeholder_type;
	} else {
		return type->return_type();
	}
}

void FunctionVersion::analyze_global_functions(SemanticAnalyzer* analyzer) {
	analyzer->enter_function((FunctionVersion*) this);
	body->analyze_global_functions(analyzer);
	analyzer->leave_function();
}

void FunctionVersion::pre_analyze(SemanticAnalyzer* analyzer, const std::vector<const Type*>& args) {
	const Environment& env = analyzer->env;
	if (pre_analyzed) return;
	pre_analyzed = true;
	// std::cout << "FunctionVersion " << body.get() << " ::pre_analyze " << args << std::endl;
	analyzer->enter_function((FunctionVersion*) this);
	parent->current_version = this;
	// Create arguments
	for (unsigned i = 0; i < parent->arguments.size(); ++i) {
		auto type = i < args.size() ? args.at(i) : (i < parent->defaultValues.size() && parent->defaultValues.at(i) != nullptr ? parent->defaultValues.at(i)->type : env.any);
		auto name = parent->arguments.at(i)->content;
		auto arg = new Variable(name, parent->arguments.at(i), VarScope::PARAMETER, type, i, nullptr, analyzer->current_function(), nullptr, nullptr, nullptr);
		arguments.emplace(name, arg);
		initial_arguments.emplace(name, arg);
	}
	body->pre_analyze(analyzer);

	analyzer->leave_function();
}

void FunctionVersion::analyze(SemanticAnalyzer* analyzer, const std::vector<const Type*>& args) {
	// std::cout << "Function::analyse_body(" << args << ")" << std::endl;
	auto& env = analyzer->env;

	parent->current_version = this;
	analyzer->enter_function((FunctionVersion*) this);

	// Prepare the placeholder return type for recursive functions
	type = Type::fun(getReturnType(analyzer->env), args, parent)->pointer();

	// Captures
	for (auto& capture : captures_inside) {
		capture->type = capture->parent->type;
		// std::cout << "analyze capture " << capture << " " << capture->type << std::endl;
	}

	// Arguments
	std::vector<const Type*> arg_types;
	for (unsigned i = 0; i < parent->arguments.size(); ++i) {
		auto type = i < args.size() ? args.at(i) : (i < parent->defaultValues.size() && parent->defaultValues.at(i) != nullptr ? parent->defaultValues.at(i)->type : env.any);
		arg_types.push_back(type);
	}

	body->analyze(analyzer);

	auto return_type = env.void_;
	// std::cout << "body->type " << body->type << std::endl;
	// std::cout << "body->return_type " << body->return_type << std::endl;
	if (auto c = dynamic_cast<const Compound_type*>(body->type)) {
		for (const auto& t : c->types) {
			if (t != placeholder_type) {
				return_type = return_type->operator + (t);
			}
		}
	} else {
		return_type = body->type;
	}
	if (auto c = dynamic_cast<const Compound_type*>(body->return_type)) {
		for (const auto& t : c->types) {
			if (t != placeholder_type) {
				return_type = return_type->operator + (t);
			}
		}
	} else {
		return_type = return_type->operator + (body->return_type);
	}
	// std::cout << "body->type " << body->type << std::endl;
	// std::cout << "body->return_type " << body->return_type << std::endl;
	if (body->type->temporary or body->return_type->temporary) return_type = return_type->add_temporary();
	// std::cout << "return type = " << return_type << std::endl;

	// Default version of the function, the return type must be any
	if (not return_type->is_void() and not parent->is_main_function and this == parent->default_version.get() and parent->generate_default_version) {
		return_type = env.any;
	}
	if (return_type->is_function()) {
		return_type = return_type->pointer();
	}
	// std::cout << "return_type " << return_type << std::endl;
	body->type = return_type;
	if (parent->captures.size()) {
		type = Type::closure(return_type, arg_types, parent);
	} else {
		type = Type::fun(return_type, arg_types, parent)->pointer();
	}

	// Re-analyse the recursive function to clean the placeholder types
	if (recursive) {
		body->analyze(analyzer);
	}

	analyzer->leave_function();

	// std::cout << "function analysed body : " << version->type << std::endl;
}

Variable* FunctionVersion::capture(SemanticAnalyzer* analyzer, Variable* var) {
	// std::cout << "Function::capture " << var << std::endl;
	auto& env = analyzer->env;

	// the function becomes a closure
	type = Type::closure(getReturnType(env), type->arguments());
	// Already exists?
	for (size_t i = 0; i < parent->captures.size(); ++i) {
		if (parent->captures[i]->name == var->name) {
			// std::cout << "Capture already exists" << std::endl;
			auto capture = analyzer->update_var(parent->captures[i]); // Copy : one var outside the function, one inside
			capture->injected = true;
			capture->scope = VarScope::CAPTURE;
			captures_inside.push_back(capture);
			return capture;
		}
	}

	// Capture from direct parent
	// std::cout << "var->function->parent " << (void*)var->function->parent << " parent->parent " << (void*)parent->parent << std::endl;
	if (var->function->parent == parent->parent) {
		// std::cout << "Capture from parent" << std::endl;
		auto parent_version = parent->parent->current_version ? parent->parent->current_version : parent->parent->default_version.get();
		analyzer->enter_function(parent_version);
		analyzer->enter_block(parent_version->body.get());
		analyzer->enter_section(parent_version->body->sections.front().get());
		auto converted_var = analyzer->update_var(var);
		converted_var->injected = true;
		analyzer->leave_section();
		analyzer->leave_block();
		analyzer->leave_function();

		converted_var->index = parent->captures.size();
		parent->captures.push_back(converted_var);
		parent->captures_map.insert({ var->name, converted_var });

		auto capture = analyzer->update_var(converted_var);
		capture->scope = VarScope::CAPTURE;
		captures_inside.push_back(capture);
		// std::cout << "Capture " << capture << " " << (void*) capture << " parent " << capture->parent << " " << (void*) capture->parent << " " << (int) capture->scope << std::endl;
		return capture;
	} else {
		// Capture from parent of parent
		// std::cout << "Capture from parent of parent" << std::endl;
		// std::cout << "var->function->parent " << var->function->parent << std::endl << "parent->parent " << parent->parent << std::endl << std::endl;
		// std::cout << "Capture by parent" << std::endl;
		// std::cout << "parents versions " << parent->parent->versions.size() << std::endl;
		auto parent_version = parent->parent->current_version ? parent->parent->current_version : parent->parent->default_version.get();
		analyzer->enter_function(parent_version);
		analyzer->enter_block(parent_version->body.get());
		analyzer->enter_section(parent_version->body->sections.front().get());
		auto capture_in_parent = parent_version->capture(analyzer, var);
		analyzer->leave_section();
		analyzer->leave_block();
		analyzer->leave_function();
		// std::cout << "capture in parent : " << capture_in_parent << std::endl;
		auto c = capture(analyzer, capture_in_parent);
		c->parent_index = capture_in_parent->index;
		return c;
	}
}

Completion FunctionVersion::autocomplete(SemanticAnalyzer& analyzer, size_t position) {
	analyzer.enter_function((FunctionVersion*) this);
	auto completion = body->autocomplete(analyzer, position);
	analyzer.leave_function();
	return completion;
}

Hover FunctionVersion::hover(SemanticAnalyzer& analyzer, size_t position) {
	return body->hover(analyzer, position);
}

#if COMPILER

bool FunctionVersion::is_compiled() const {
	return fun.v != nullptr;
}
void FunctionVersion::create_function(Compiler& c) {
	if (fun.v) return;
	auto& env = c.env;

	std::vector<const Type*> args;
	if (parent->captures.size()) {
		args.push_back(env.any); // first arg is the function pointer
	}
	for (auto& t : this->type->arguments()) {
		args.push_back(t);
	}
	auto function_type = Type::fun(type->return_type(), args);
	auto fun_name = parent->is_main_function ? "main" : parent->name;
	auto f = llvm::Function::Create((llvm::FunctionType*) function_type->llvm(c), llvm::Function::InternalLinkage, fun_name, c.program->module);
	fun = { f, function_type->pointer() };
	assert(c.check_value(fun));

	if (body->throws) {
		auto personalityfn = c.program->module->getFunction("__gxx_personality_v0");
		if (!personalityfn) {
			personalityfn = llvm::Function::Create(llvm::FunctionType::get(llvm::Type::getInt32Ty(c.getContext()), true), llvm::Function::ExternalLinkage, "__gxx_personality_v0", c.program->module);
		}
		f->setPersonalityFn(personalityfn);
	}
	// body->sections.front()->pre_compile(c);
	block = body->sections.front()->basic_block;
	// std::cout << "function block " << block << std::endl;
}

Compiler::value FunctionVersion::compile(Compiler& c, bool compile_body) {
	// std::cout << "Function::Version " << parent->name << "::compile(" << type << ", compile_body=" << compile_body << ")" << std::endl;
	// std::cout << "Captures : " << captures.size() << " : ";
	// for (const auto& c : captures) std::cout << c->name << " " << c->type << " " << (int)c->scope << " ";
	// std::cout << std::endl;

	if (not is_compiled()) {

		parent->compile_captures(c);

		std::vector<llvm::Type*> args;
		if (parent->captures.size()) {
			args.push_back(c.env.any->llvm(c)); // first arg is the function pointer
		}
		for (auto& t : this->type->arguments()) {
			args.push_back(t->llvm(c));
		}
		// Create the llvm function
		create_function(c);

		c.enter_function((llvm::Function*) fun.v, parent->captures.size() > 0, this);

		c.enter_section(body->sections.front().get(), false);
		// c.builder.SetInsertPoint(block);

		// Declare context vars
		if (parent->is_main_function and c.vm->context) {
			for (const auto& var : c.vm->context->vars) {
				// std::cout << "Main function compile context var " <<  var.first << " " << (void*)var.second.variable << std::endl;
				c.add_external_var(var.second.variable);
			}
		}

		// Create arguments
		unsigned index = 0;
		int offset = parent->captures.size() ? -1 : 0;
		for (auto& arg : ((llvm::Function*) fun.v)->args()) {
			if (index == 0 && parent->captures.size()) {
				arg.setName("closure");
			} else if (offset + index < parent->arguments.size()) {
				const auto name = parent->arguments.at(offset + index)->content;
				const auto& argument = initial_arguments.at(name);
				// std::cout << "create entry of argument " << argument << " " << (void*)argument << std::endl;
				const auto type = this->type->arguments().at(offset + index)->not_temporary();
				arg.setName(name);
				argument->create_entry(c);
				if (type->is_mpz_ptr()) {
					c.insn_store(argument->entry, c.insn_load({&arg, type}));
				} else {
					if (this->type->arguments().at(offset + index)->temporary) {
						Compiler::value a = {&arg, type};
						c.insn_inc_refs(a);
						c.insn_store(argument->entry, a);
					} else {
						c.insn_store(argument->entry, {&arg, type});
					}
				}
			}
			index++;
		}

		// Create captures variables
		for (const auto& capture : captures_inside) {
			// capture->create_entry(c);
			// capture->store_value(c, c.insn_get_capture(capture->index, c.env.any));
			capture->entry = c.insn_get_capture_l(capture->index, c.env.any);
			// std::cout << "create capture " << capture << " " << (void*) capture << " from get_capture_l " << (void*) capture->entry.v << std::endl;
		}

		c.leave_section(false);

		if (compile_body) {
			body->compile(c);
			body->compile_end(c);
		} else {
			compile_return(c, { c.env });
		}

		if (!parent->is_main_function) {
			c.leave_function();
			// Create a function : 1 op
			c.inc_ops(1);
		}
		llvm::verifyFunction(*((llvm::Function*) fun.v));
	}

	if (parent->captures.size()) {
		std::vector<Compiler::value> captures;
		if (!parent->is_main_function) {
			for (const auto& capture : parent->captures) {
				Compiler::value jit_cap { c.env };
				// std::cout << "Compile capture " << capture << " " << (int) capture->scope << std::endl;
				if (capture->parent->scope == VarScope::LOCAL) {
					auto f = dynamic_cast<Function*>(capture->value);
					if (f) {
						jit_cap = f->compile_version(c, capture->version);
					} else {
						jit_cap = capture->get_value(c);
					}
				} else if (capture->parent->scope == VarScope::CAPTURE) {
					jit_cap = capture->get_value(c);
				} else {
					jit_cap = capture->get_value(c);
				}
				assert(jit_cap.t->is_polymorphic());
				captures.push_back(jit_cap);
			}
		}
		value = c.new_closure(fun, captures);
	} else {
		// if (fun.t->is_pointer()) {
			value = fun;
		// } else {
			// value = { c.builder.CreatePointerCast(fun.v, fun.t->pointer()->llvm(c)), fun.t->pointer() };
		// }
	}
	// std::cout << "Function '" << parent->name << "' compiled: " << value.t << std::endl;
	return value;
}

void FunctionVersion::compile_return(Compiler& c, Compiler::value v, bool delete_variables) const {
	assert(c.check_value(v));
	// Delete temporary mpz arguments
	for (size_t i = 0; i < type->arguments().size(); ++i) {
		const auto& name = parent->arguments.at(i)->content;
		const auto& arg = initial_arguments.at(name);
		const auto& arg2 = arguments.at(name);
		if (arg2->entry.v == arg->entry.v) {
			// std::cout << "delete tmp argument " << name << " " << type->argument(i) << std::endl;
			if (type->argument(i) == c.env.tmp_mpz_ptr) {
				c.insn_delete_mpz(arg->entry);
			} else if (type->argument(i)->temporary) {
				c.insn_delete(c.insn_load(arg->entry));
			}
		} else {
			if (arg2->entry.v) {
				// std::cout << "delete argument " << arg2 << " " << arg2->type << " " << arg2->val.t << std::endl;
				c.insn_delete_variable(arg2->entry);
			}
		}
	}
	// Delete function variables if needed
	if (delete_variables) {
		c.delete_function_variables();
	}
	// Return the value
	if (type->return_type()->is_void()) {
		c.insn_return_void();
	} else {
		auto return_type = ((FunctionVersion*) this)->getReturnType(c.env)->fold();
		// std::cout << "return type " << return_type << std::endl;
		if (v.t->is_void()) {
			if (return_type->is_bool()) v = c.new_bool(false);
			else if (return_type->is_real()) v = c.new_real(0);
			else if (return_type->is_long()) v = c.new_long(0);
			else if (return_type->is_integer()) v = c.new_integer(0);
			// else if (return_type->is_raw_function()) v = c.new_null_pointer();
			else v = c.new_null();
		}
		if (return_type->is_any()) {
			v = c.insn_convert(v, return_type);
		}
		// if (return_type->is_function()) {
		// 	std::cout << "return type function " << std::endl;
		// 	v = { c.builder.CreatePointerCast(v.v, return_type->pointer()->llvm(c)), return_type->pointer() };
		// }
		assert(c.check_value(v));
		c.insn_return(v);
	}
}

llvm::BasicBlock* FunctionVersion::get_landing_pad(Compiler& c) {
	auto catch_block = llvm::BasicBlock::Create(c.getContext(), "catch", c.F);
	auto savedIP = c.builder.saveAndClearIP();
	auto landing_pad = llvm::BasicBlock::Create(c.getContext(), "lpad", c.F);
	c.builder.SetInsertPoint(landing_pad);
	auto catchAllSelector = llvm::ConstantPointerNull::get(llvm::Type::getInt8PtrTy(c.getContext()));
	auto landingPadInst = c.builder.CreateLandingPad(llvm::StructType::get(llvm::Type::getInt64Ty(c.getContext()), llvm::Type::getInt32Ty(c.getContext())), 1);
	auto LPadExn = c.builder.CreateExtractValue(landingPadInst, 0);
	auto exception_slot = c.CreateEntryBlockAlloca("exn.slot", llvm::Type::getInt64Ty(c.getContext()));
	auto exception_line_slot = c.CreateEntryBlockAlloca("exnline.slot", llvm::Type::getInt64Ty(c.getContext()));
	c.builder.CreateStore(LPadExn, exception_slot);
	c.builder.CreateStore(c.new_long(c.exception_line.top()).v, exception_line_slot);
	landingPadInst->addClause(catchAllSelector);
	auto catcher = c.find_catcher();
	if (catcher) {
		c.builder.CreateBr(catcher->handler);
		// c.insn_call(c.env.void_, {}, "__cxa_end_catch");
	} else {
		// Catch block
		// auto catch_switch = c.builder.CreateCatchSwitch(landingPadInst, landing_pad, 1);
		// auto catchpad = c.builder.CreateCatchPad(catch_switch, {});
		auto savedIPc = c.builder.saveAndClearIP();
		c.builder.SetInsertPoint(catch_block);
		c.delete_function_variables();
		Compiler::value exception = {c.builder.CreateLoad(exception_slot), c.env.long_};
		Compiler::value exception_line = {c.builder.CreateLoad(exception_line_slot), c.env.long_};
		auto file = c.new_const_string(parent->token->location.file->path);
		auto function_name = c.new_const_string(parent->name);

		// auto eh_exception_type = llvm::FunctionType::get(c.env.i8_ptr->llvm(c), {}, false);
		// auto eh_exception = llvm::Intrinsic::getDeclaration(c.program->module, llvm::Intrinsic::eh_exceptionpointer, c.env.i8_ptr->llvm(c));
		// Compiler::value adjusted = { c.builder.CreateCall(eh_exception, { catch_block }), c.env.i8_ptr };

		auto adjusted = c.insn_call(c.env.long_, {exception}, "__cxa_begin_catch");
		auto new_ex = c.insn_call(c.env.i8_ptr, {c.new_integer(sizeof(vm::ExceptionObj))}, "__cxa_allocate_exception");
		c.insn_call(c.env.void_, {new_ex, adjusted, file, function_name, exception_line}, "System.exception_fill");
		// c.insn_call(c.env.void_, {exception}, "System.delete_exception");
		// c.insn_call(c.env.void_, {}, "__cxa_rethrow");
		c.insn_call(c.env.void_, {}, "__cxa_end_catch");
		c.insn_call(c.env.void_, {new_ex, c.new_long((long) &typeid(vm::ExceptionObj)), c.get_symbol("System.delete_exception", c.env.i8_ptr) }, "__cxa_throw");
		// c.insn_call(c.env.void_, {}, "llvm.eh.resume");
		// c.insn_call(c.env.void_, {}, "_Unwind_Resume");
		// c.builder.CreateResume(landingPadInst);
		// c.fun->compile_return(c, { c.env });
		c.builder.CreateUnreachable();

		c.builder.restoreIP(savedIPc);
		c.builder.CreateBr(catch_block);
	}
	c.builder.restoreIP(savedIP);
	return landing_pad;
}
#endif

}