#ifndef CONVERSION_HPP
#define CONVERSION_HPP

namespace ls {

class Variable;
class Section;

class Conversion {
public:
	Variable* old_parent;
	Variable* new_parent;
	Variable* child;
	Section* section;
};

}

#endif