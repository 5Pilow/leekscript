#ifndef SECTION_HPP
#define SECTION_HPP

#include <vector>
#include <memory>
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
	std::unordered_map<std::string, Variable*> variables;
	std::vector<Phi*> phis;
	std::vector<Conversion> conversions;
	bool is_end_of_block = false;
    #if COMPILER
	llvm::BasicBlock* first_basic_block = nullptr;
	llvm::BasicBlock* basic_block = nullptr;
    Compiler::value condition;
    bool added = false;
    bool left = false;
    #endif

    Section(Environment& env, std::string name);
    Section(Environment& env, std::string name, Section* predecessor);

    void add_successor(Section* successor);
    void add_predecessor(Section* predecessor);

    void add_conversion(Conversion conversion);

    void print(std::ostream& os, int indent, PrintOptions options) const;

    void pre_analyze(SemanticAnalyzer* analyzer);
    void analyze(SemanticAnalyzer* analyzer);
    void analyze_end(SemanticAnalyzer* analyzer);
    void reanalyze_conversions(SemanticAnalyzer* analyzer);

    #if COMPILER
    void pre_compile(Compiler& c);
    Compiler::value compile(Compiler& c) const;
    void compile_end(Compiler& c) const;
    #endif
};

}

#endif