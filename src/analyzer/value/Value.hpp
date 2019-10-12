#ifndef VALUE_HPP
#define VALUE_HPP

#include <map>
#include <vector>
#include <memory>
#include <string>
#include "../../constants.h"
#include "../lexical/Location.hpp"
#include "../semantic/Call.hpp"
#include "../semantic/Assignment.hpp"
#include "../PrintOptions.hpp"
#if COMPILER
#include "../../compiler/Compiler.hpp"
#endif

namespace ls {

class SemanticAnalyzer;
class Environment;

class Value {
public:
	const Type* type = nullptr;
	const Type* return_type = nullptr;
	bool returning = false;
	bool may_return = false;
	std::vector<const Type*> version;
	bool has_version = false;
	bool constant;
	bool parenthesis = false;
	std::string default_version_fun;
	std::map<std::vector<const Type*>, std::string> versions;
	bool is_void = false;
	bool throws = false;
	bool jumping = false; // Indicates that the value contains a jump
	Section* end_section = nullptr;

	Value() = delete;
	Value(Environment& env);
	virtual ~Value() {}

	virtual bool isLeftValue() const;
	virtual bool is_zero() const;

	// TODO PrintOptions to merge parameters
	virtual void print(std::ostream&, int indent = 0, PrintOptions options = {}) const = 0;
	std::string to_string() const;

	virtual Location location() const = 0;

	virtual void set_end_section(Section* end_section);

	virtual void pre_analyze(SemanticAnalyzer*);

	virtual const Type* will_take(SemanticAnalyzer*, const std::vector<const Type*>& args_type, int level);
	virtual bool will_store(SemanticAnalyzer*, const Type*);
	virtual bool elements_will_store(SemanticAnalyzer*, const Type*, int level);
	virtual bool must_be_any(SemanticAnalyzer*);
	virtual void must_return_any(SemanticAnalyzer*);
	virtual void set_version(SemanticAnalyzer*, const std::vector<const Type*>&, int level);
	virtual const Type* version_type(std::vector<const Type*>) const;
	virtual Call get_callable(SemanticAnalyzer*, int argument_count) const;
	virtual void analyze(SemanticAnalyzer*);

	#if COMPILER
	virtual Compiler::value compile(Compiler&) const = 0;
	virtual Compiler::value compile_version(Compiler&, std::vector<const Type*>) const;
	virtual void compile_end(Compiler&) const {}
	#endif

	virtual std::unique_ptr<Value> clone(Block* parent) const = 0;

	static std::string tabs(int indent);
};

std::ostream& operator << (std::ostream& os, const Value* v);

}

template <typename T_DEST, typename T_SRC>
inline std::unique_ptr<T_DEST> unique_static_cast(std::unique_ptr<T_SRC>&& src) {
	auto dest_ptr = static_cast<T_DEST*>(src.get());
	src.release();
	return std::unique_ptr<T_DEST>(dest_ptr);
}

#endif
