#ifndef MUTATION_HPP
#define MUTATION_HPP

namespace ls {

class Variable;
class Section;

class Mutation {
public:
	Variable* variable;
	Section* section;
};

}

#endif