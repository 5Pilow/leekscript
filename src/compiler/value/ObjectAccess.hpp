#ifndef OBJECTACCESS_HPP
#define OBJECTACCESS_HPP

#include <string>

#include "../../compiler/value/LeftValue.hpp"
#include "../../compiler/value/Value.hpp"
#include "../../vm/value/LSString.hpp"
#include "../lexical/Token.hpp"

namespace ls {

class ObjectAccess : public LeftValue {
public:

	Value* object;
	std::unique_ptr<Token> field;
	std::string object_class_name;
	std::string class_name;
	bool class_attr = false;
	void* attr_addr;
	void* static_access_function = nullptr;
	void* access_function = nullptr;
	Type field_type;

	ObjectAccess(Token* token);
	virtual ~ObjectAccess();

	virtual void print(std::ostream&, int indent, bool debug) const override;

	virtual void analyse(SemanticAnalyser*, const Type&) override;
	virtual void change_type(SemanticAnalyser*, const Type&) override;

	virtual Compiler::value compile(Compiler&) const override;

	virtual Compiler::value compile_l(Compiler&) const override;
};

}

#endif
