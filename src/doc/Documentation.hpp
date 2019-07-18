#ifndef VM_DOC_DOCUMENTATION_HPP_
#define VM_DOC_DOCUMENTATION_HPP_

#include <vector>
#include <ostream>
#include "../standard/Module.hpp"

namespace ls {

class Documentation {
public:

	Documentation();
	virtual ~Documentation();

	void generate(VM* vm, std::ostream& os, std::string lang = "fr");
};

}

#endif
