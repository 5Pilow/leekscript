#include "../../compiler/instruction/ClassDeclaration.hpp"

#include "../../vm/value/LSNull.hpp"
#include "../../vm/value/LSClass.hpp"

using namespace std;

namespace ls {

ClassDeclaration::ClassDeclaration(Token* token) {
	name = token->content;
	this->token.reset(token);
	var = nullptr;
	ls_class = new LSClass(name);
}

ClassDeclaration::~ClassDeclaration() {
	for (const auto& vd : fields) {
		delete vd;
	}
	delete ls_class;
}

void ClassDeclaration::print(ostream& os, int indent, bool debug) const {
	os << "class " << name << " {" << endl;
	for (VariableDeclaration* vd : fields) {
		vd->print(os, indent + 1, debug);
		os << endl;
	}
	os << "}";
}

void ClassDeclaration::analyse(SemanticAnalyser* analyser, const Type&) {

	var = analyser->add_var(token.get(), Type::CLASS, nullptr, nullptr);

	for (auto vd : fields) {
		vd->analyse(analyser, Type::UNKNOWN);
	}
}

Compiler::value ClassDeclaration::compile(Compiler& c) const {

	auto clazz = c.new_pointer(ls_class);

	c.add_var(name, clazz.v, Type::CLASS, true);

	return clazz;
}

}
