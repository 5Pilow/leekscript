#ifndef COMPILER_HPP
#define COMPILER_HPP

#include "llvm/ADT/APFloat.h"
#include "llvm/ADT/APInt.h"
#include "llvm/ADT/STLExtras.h"
#include "llvm/IR/BasicBlock.h"
#include "llvm/IR/Constants.h"
#include "llvm/IR/DerivedTypes.h"
#include "llvm/IR/Function.h"
#include "llvm/IR/IRBuilder.h"
#include "llvm/IR/LLVMContext.h"
#include "llvm/IR/LegacyPassManager.h"
#include "llvm/IR/Module.h"
#include "llvm/IR/Type.h"
#include "llvm/IR/Verifier.h"
#include "llvm/Support/TargetSelect.h"
#include "llvm/Target/TargetMachine.h"
#include "llvm/Transforms/Scalar.h"
#include "llvm/Transforms/Scalar/GVN.h"
#include "llvm/ADT/STLExtras.h"
#include "llvm-c/Transforms/Scalar.h"
#include "llvm/Analysis/BasicAliasAnalysis.h"
#include "llvm/ExecutionEngine/ExecutionEngine.h"
#include "llvm/ExecutionEngine/JITSymbol.h"
#include "llvm/ExecutionEngine/RTDyldMemoryManager.h"
#include "llvm/ExecutionEngine/SectionMemoryManager.h"
#include "llvm/ExecutionEngine/Orc/CompileUtils.h"
#include "llvm/ExecutionEngine/Orc/IRCompileLayer.h"
#include "llvm/ExecutionEngine/Orc/LambdaResolver.h"
#include "llvm/ExecutionEngine/Orc/RTDyldObjectLinkingLayer.h"
#include "llvm/ExecutionEngine/Orc/GlobalMappingLayer.h"
#include "llvm/ExecutionEngine/Orc/ExecutionUtils.h"
#include "llvm/ExecutionEngine/Orc/IRTransformLayer.h"
#include "llvm/ExecutionEngine/Orc/ThreadSafeModule.h"
#include "llvm/Transforms/InstCombine/InstCombine.h"
#include "llvm/ExecutionEngine/Orc/IndirectionUtils.h"
#include "llvm/IR/DataLayout.h"
#include "llvm/IR/Mangler.h"
#include "llvm/Support/DynamicLibrary.h"
#include "llvm/Support/raw_ostream.h"
#include "llvm/Target/TargetMachine.h"
#include "../vm/Exception.hpp"
#include "../vm/LSValue.hpp"
#include <gmp.h>

namespace ls {

class Program;
class VM;
class Function;
class Type;
class FunctionVersion;
class Variable;
class Block;
class Environment;
class Section;

class Compiler {
public:
	struct value {
		llvm::Value* v;
		const Type* t;
		bool operator == (const value& o) const {
			return v == o.v and t == o.t;
		}
		bool operator != (const value& o) const {
			return v != o.v or t != o.t;
		}
		value(Environment& env);
		value(llvm::Value* v, const Type* t) : v(v), t(t) {}
	};
	struct label {
		llvm::BasicBlock* block = nullptr;
	};
	struct catcher {
		llvm::BasicBlock* handler;
	};
	struct function_entry {
		llvm::JITTargetAddress addr;
		llvm::Function* function;
	};

	Environment& env;
	llvm::orc::ThreadSafeContext Ctx;
	llvm::IRBuilder<> builder;
	llvm::Function* F;
	FunctionVersion* fun;
	std::stack<llvm::Function*> functions;
	std::stack<FunctionVersion*> functions2;
	std::stack<bool> function_is_closure;
	std::vector<int> functions_blocks;
	std::stack<llvm::BasicBlock*> function_llvm_blocks;
	std::vector<int> loops_blocks; // how many blocks are open in the current loop
	std::vector<Section*> loops_end_sections;
	std::vector<Section*> loops_cond_sections;
	std::vector<std::vector<Block*>> blocks;
	std::vector<std::vector<Section*>> sections;
	std::vector<std::vector<std::vector<catcher>>> catchers;
	std::map<std::pair<std::string, const Type*>, function_entry> mappings;
	std::stack<int> exception_line;
	bool export_bitcode = false;
	bool export_optimized_ir = false;
	std::unordered_map<std::string, Compiler::value> global_strings;

	VM* vm;
	Program* program;

	std::unique_ptr<llvm::TargetMachine> TM;
	llvm::DataLayout DL;
	llvm::orc::ExecutionSession ES;
	llvm::orc::LegacyRTDyldObjectLinkingLayer ObjectLayer;
	llvm::orc::LegacyIRCompileLayer<decltype(ObjectLayer), llvm::orc::SimpleCompiler> CompileLayer;
	using OptimizeFunction = std::function<std::unique_ptr<llvm::Module>(std::unique_ptr<llvm::Module>)>;
	llvm::orc::LegacyIRTransformLayer<decltype(CompileLayer), OptimizeFunction> OptimizeLayer;

	Compiler(Environment& env, VM* vm);

	const llvm::DataLayout& getDataLayout() const { return DL; }

	llvm::LLVMContext& getContext() { return *Ctx.getContext(); }

	std::unique_ptr<llvm::Module> optimizeModule(std::unique_ptr<llvm::Module> M);
	llvm::orc::VModuleKey addModule(std::unique_ptr<llvm::Module> M, bool optimize, bool export_bitcode = false, bool export_optimized_ir = false);

	llvm::JITSymbol findSymbol(const std::string Name) {
		return OptimizeLayer.findSymbol(Name, false);
	}
	void removeModule(llvm::orc::VModuleKey K) {
		cantFail(OptimizeLayer.removeModule(K));
	}

	/// CreateEntryBlockAlloca - Create an alloca instruction in the entry block of the function.  This is used for mutable variables etc.
	llvm::AllocaInst* CreateEntryBlockAlloca(const std::string& VarName, llvm::Type* type) const;

	void init();
	void end();

	// Value creation
	value clone(value);
	value new_null();
	value new_bool(bool b);
	value new_integer(int i);
	value new_real(double r);
	value new_long(long l);
	value new_mpz();
	value new_const_string(std::string s);
	value new_null_pointer(const Type* type);
	value new_function(const Type* type);
	value new_function(Compiler::value fun);
	value new_function(std::string name, const Type* type);
	value new_closure(Compiler::value fun, std::vector<value> captures);
	value new_class(std::string name);
	value new_object();
	value new_object_class(value clazz);
	value new_set();
	value create_entry(const std::string& name, const Type* type);
	value get_symbol(const std::string& name, const Type* type);
	value get_vm();

	// Conversions
	value to_int(value);
	value to_real(value, bool delete_temporary = false);
	value to_long(value);
	value insn_convert(value, const Type*, bool delete_temporary = false);
	value to_numeric(value);

	// Operators wrapping
	value insn_not(value);
	value insn_not_bool(value);
	value insn_neg(value);
	value insn_and(value, value);
	value insn_or(value, value);
	value insn_add(value, value);
	value insn_sub(value, value);
	value insn_eq(value, value);
	value insn_pointer_eq(value, value);
	value insn_ne(value, value);
	value insn_lt(value, value);
	value insn_le(value, value);
	value insn_gt(value, value);
	value insn_ge(value, value);
	value insn_mul(value, value);
	value insn_div(value, value);
	value insn_int_div(value, value);
	value insn_bit_and(value, value);
	value insn_bit_or(value, value);
	value insn_bit_xor(value, value);
	value insn_shl(value, value);
	value insn_lshr(value, value);
	value insn_ashr(value, value);
	value insn_mod(value, value, bool check_overflow = true);
	value insn_double_mod(value, value);
	value insn_cmpl(value, value);

	// Math Functions
	value insn_log(value);
	value insn_log10(value);
	value insn_ceil(value);
	value insn_round(value);
	value insn_floor(value);
	value insn_cos(value);
	value insn_sin(value);
	value insn_tan(value);
	value insn_acos(value);
	value insn_asin(value);
	value insn_atan(value);
	value insn_pow(value, value);
	value insn_min(value, value);
	value insn_max(value, value);
	value insn_exp(value);
	value insn_atan2(value, value);
	value insn_abs(value);

	// Value management
	value insn_to_any(value v);
	value insn_to_bool(value v);
	value insn_load(value v);
	value insn_load_member(value v, int pos);
	void  insn_store(value, value);
	void  insn_store_member(value, int, value);
	value insn_typeof(value v);
	value insn_class_of(value v);
	void  insn_delete(value v);
	void  insn_delete_variable(value v);
	void  insn_delete_temporary(value v);
	value insn_get_capture(int index, const Type* type);
	value insn_get_capture_l(int index, const Type* type);
	value insn_move_inc(value);
	value insn_clone_mpz(value mpz);
	void  insn_delete_mpz(value mpz);
	value insn_inc_refs(value v);
	value insn_move(value v);
	value insn_refs(value v);
	value insn_native(value v);

	// Arrays
	value new_array(const Type* type, std::vector<value> elements);
	value insn_array_size(value v);
	void  insn_push_array(value array, value element);
	value insn_array_at(value array, value index);
	value insn_array_end(value array);

	// Iterators
	value iterator_begin(value v);
	value iterator_rbegin(value v);
	value iterator_end(value v, value it);
	value iterator_rend(value v, value it);
	value iterator_get(const Type* collectionType, value it, value previous);
	value iterator_rget(const Type* collectionType, value it, value previous);
	value iterator_key(value v, value it, value previous);
	value iterator_rkey(value v, value it, value previous);
	void iterator_increment(const Type* collectionType, value it);
	void iterator_rincrement(const Type* collectionType, value it);
	value insn_foreach(value v, const Type* output, Variable* var, Variable* key, std::function<value(value, value)>, bool reversed = false, std::function<value(value, value)> body2 = nullptr);

	// Controls
	label insn_init_label(std::string name);
	void insn_if(value v, std::function<void()> then, std::function<void()> elze = nullptr);
	void insn_if_new(value cond, label* then, label* elze);
	void insn_if_sections(value cond, Section* then, Section* elze);
	void insn_if_not(value v, std::function<void()> then);
	void insn_throw(value v);
	void insn_throw_object(vm::Exception type);
	void insn_label(label*);
	void insn_branch(label* l);
	void insn_branch(Section* s);
	void insn_return(value v);
	void insn_return_void();
	value insn_phi(const Type* type, value v1, label l1, value v2, label l2);
	value insn_phi(const Type* type, value v1, Section* b1, value v2, Section* b2);

	// Call functions
	value insn_invoke(const Type* return_type, std::vector<value> args, std::string name);
	value insn_invoke(const Type* return_type, std::vector<value> args, value func);
	value insn_call(value fun, std::vector<value> args);
	value insn_call(const Type* return_type, std::vector<value> args, std::string name, bool readonly = false);
	void function_add_capture(value fun, value capture);
	void log(const std::string&& str);

	// Blocks
	void enter_block(Block* block);
	void leave_block(bool delete_vars = true);
	void enter_section(Section* section);
	void leave_section();
	void leave_section_condition(Compiler::value condition);
	void delete_variables_block(int deepness); // delete all variables in the #deepness current blocks
	void enter_function(llvm::Function* F, bool is_closure, FunctionVersion* fun);
	void leave_function();
	int get_current_function_blocks() const;
	void delete_function_variables();
	bool is_current_function_closure() const;
	void insert_new_generation_block();
	Block* current_block() const;
	Section* current_section() const;

	// Variables
	value add_external_var(Variable*);
	void export_context_variable(const std::string& name, Compiler::value v);
	void add_temporary_variable(Variable* variable);
	void add_temporary_value(value);
	void pop_temporary_value();
	void add_temporary_expression_value(value);
	void pop_temporary_expression_value();

	// Loops
	void enter_loop(Section*, Section*);
	void leave_loop();
	Section* get_current_loop_end_section(int deepness) const;
	Section* get_current_loop_cond_section(int deepness) const;
	int get_current_loop_blocks(int deepness) const;

	/** Operations **/
	void inc_ops(int add);
	void inc_ops_jit(value add);

	/** Exceptions **/
	void mark_offset(int line);
	void insn_try_catch(std::function<void()> try_, std::function<void()> catch_);
	void insn_check_args(std::vector<value> args, std::vector<LSValueType> types);
	const catcher* find_catcher() const;

	// Utils
	static void print_mpz(__mpz_struct value);
	bool check_value(value) const;
	void increment_mpz_created();
	void increment_mpz_deleted();
};

}

namespace std {
	std::ostream& operator << (std::ostream&, const __mpz_struct);
	std::ostream& operator << (std::ostream&, const std::vector<ls::Compiler::value>&);
}

#endif
