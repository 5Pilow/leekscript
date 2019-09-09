#ifndef OBJECTACCESS_HPP
#define OBJECTACCESS_HPP

#include <string>
#include "LeftValue.hpp"
#include "Value.hpp"
#include "../../vm/value/LSString.hpp"
#include "../lexical/Token.hpp"

namespace ls {

class Method;

class ObjectAccess : public LeftValue {
public:

	std::unique_ptr<Value> object;
	Token* field;
	std::string object_class_name;
	std::string class_name;
	bool class_method = false;
	Call* call = nullptr;
	bool class_field = false;
	void* attr_addr = nullptr;
	std::function<Compiler::value(Compiler&)> static_access_function = nullptr;
	std::function<Compiler::value(Compiler&, Compiler::value)> access_function = nullptr;
	std::string native_access_function = "";
	std::string native_static_access_function = "";
	const Type* field_type;
	LSFunction* ls_function = nullptr;

	ObjectAccess(Token* token);
	virtual ~ObjectAccess();

	virtual bool isLeftValue() const override;
	
	virtual void print(std::ostream&, int indent, bool debug, bool condensed) const override;
	virtual Location location() const override;

	void set_version(SemanticAnalyzer*, const std::vector<const Type*>& args, int level) override;
	const Type* will_take(SemanticAnalyzer*, const std::vector<const Type*>&, int level) override;
	virtual Call get_callable(SemanticAnalyzer*, int argument_count) const;
	virtual const Type* version_type(std::vector<const Type*>) const override;
	virtual void pre_analyze(SemanticAnalyzer*) override;
	virtual void analyze(SemanticAnalyzer*) override;

	virtual Compiler::value compile(Compiler&) const override;
	virtual Compiler::value compile_version(Compiler& c, std::vector<const Type*> version) const;
	virtual Compiler::value compile_l(Compiler&) const override;

	virtual std::unique_ptr<Value> clone() const override;
};

}

#endif
