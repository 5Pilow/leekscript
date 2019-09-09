#ifndef VALUE_HPP
#define VALUE_HPP

#include <map>
#include <vector>
#include "../Compiler.hpp"
#include "../lexical/Location.hpp"
#include "../semantic/Call.hpp"
#include "../semantic/Assignment.hpp"

namespace ls {

class SemanticAnalyzer;

class Value {
public:
	const Type* type;
	const Type* return_type;
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

	Value();
	virtual ~Value() {}

	virtual bool isLeftValue() const;
	virtual bool is_zero() const;

	// TODO PrintOptions to merge parameters
	virtual void print(std::ostream&, int indent = 0, bool debug = false, bool condensed = false) const = 0;
	std::string to_string() const;

	virtual Location location() const = 0;

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

	virtual Compiler::value compile(Compiler&) const = 0;
	virtual Compiler::value compile_version(Compiler&, std::vector<const Type*>) const;
	virtual void compile_end(Compiler&) const {}

	virtual std::unique_ptr<Value> clone() const = 0;

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
