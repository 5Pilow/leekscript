#ifndef SECTION_HPP
#define SECTION_HPP

#include <vector>
#include <memory>
#include <unordered_map>
#include "../instruction/Instruction.hpp"
#include "Conversion.hpp"

namespace ls {

class Phi;
class Mutation;

class Section {
    static const std::vector<std::string> COLORS;
    static size_t current_id;
public:
    size_t id;
    Environment& env;
    std::string color;
    std::string name;
	std::vector<Instruction*> instructions;
    std::vector<Section*> predecessors;
    std::vector<Section*> successors;
	Block* block = nullptr;
	std::unordered_map<std::string, Variable*> variables; // Variable in the section by name
	std::vector<std::unique_ptr<Variable>> variable_list; // All the variables own by the section
	std::vector<Phi*> phis;
	bool is_end_of_block = false;
    #if COMPILER
	llvm::BasicBlock* first_basic_block = nullptr;
	llvm::BasicBlock* basic_block = nullptr;
    Compiler::value condition;
    bool added = false;
    bool left = false;
    #endif

    Section(Environment& env, std::string name, Block* block = nullptr);
    Section(Environment& env, std::string name, Section* predecessor);

    void add_successor(Section* successor);
    void add_predecessor(Section* predecessor);
	Section* common_ancestor(Section* section) const;

    void print(std::ostream& os, int indent, PrintOptions options) const;

    void pre_analyze(SemanticAnalyzer* analyzer);
    void analyze(SemanticAnalyzer* analyzer);
    void analyze_end(SemanticAnalyzer* analyzer);

    #if COMPILER
    void pre_compile(Compiler& c);
    Compiler::value compile(Compiler& c) const;
    void compile_end(Compiler& c) const;
    #endif
};

}

namespace ls {
	std::ostream& operator << (std::ostream& os, const Section*);
}

#endif