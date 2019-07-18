#ifndef TYPE_MUTATOR_HPP
#define TYPE_MUTATOR_HPP

#include "../analyzer/value/Value.hpp"

namespace ls {

class SemanticAnalyzer;

class TypeMutator {
public:
	virtual void apply(SemanticAnalyzer*, std::vector<Value*> values, const Type* return_type) const = 0;
	virtual int compile(Compiler&, CallableVersion* callable, std::vector<Value*> values) const = 0;
};
class TypeExtractor {
public:
	virtual const Type* extract(SemanticAnalyzer*, std::vector<Value*> values) const;
};

class ConvertMutator : public TypeMutator {
public:
	bool store_array_size = false;
	ConvertMutator(bool store_array_size = false) : store_array_size(store_array_size) {}
	virtual void apply(SemanticAnalyzer*, std::vector<Value*> values, const Type* return_type) const override;
	virtual int compile(Compiler&, CallableVersion* callable, std::vector<Value*> values) const override;
};

class ChangeValueMutator : public TypeMutator {
public:
	ChangeValueMutator() {}
	virtual void apply(SemanticAnalyzer*, std::vector<Value*> values, const Type* return_type) const override;
	virtual int compile(Compiler&, CallableVersion* callable, std::vector<Value*> values) const override;
};

class WillTakeMutator : public TypeMutator {
public:
	int index;
	std::vector<const Type*> types;
	WillTakeMutator(int index, std::vector<const Type*> types) : index(index), types(types) {}
	virtual void apply(SemanticAnalyzer*, std::vector<Value*> values, const Type* return_type) const override;
};

class ElementExtractor : public TypeExtractor {
public:
	TypeExtractor* extractor;
	ElementExtractor(TypeExtractor* extractor) : extractor(extractor) {}
	virtual const Type* extract(SemanticAnalyzer*, std::vector<Value*> values) const override;
};
class ArgumentExtractor : public TypeExtractor {
public:
	int index;
	ArgumentExtractor(int index) : index(index) {}
	virtual const Type* extract(SemanticAnalyzer*, std::vector<Value*> values) const override;
};

}

#endif
