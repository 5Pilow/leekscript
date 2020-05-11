#ifndef LSCLOSURE_HPP
#define LSCLOSURE_HPP

#include <functional>
#include <iostream>
#include <vector>
#include <memory>
#include <map>
#include "../LSValue.hpp"
#include "LSFunction.hpp"

namespace ls {

class LSClosure : public LSFunction {
public:

	static LSClosure* constructor(VM* vm, void* f);

	std::vector<LSValue*> captures;
	std::vector<bool> captures_native; // TODO check if necessary

	LSClosure(void* function);
	virtual ~LSClosure();
	virtual bool closure() const;

	static void add_capture(LSClosure* closure, LSValue* value);
	static LSValue* get_capture(LSClosure* closure, int index);
	static LSValue** get_capture_l(LSClosure* closure, int index);
};

template <class R, class ...A> R call(LSClosure* function, A... args) {
	auto fun = (R (*)(void*, A...)) function->function;
	return fun(function, args...);
}

}

#endif
