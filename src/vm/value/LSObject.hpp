#ifndef LSOBJECT_HPP_
#define LSOBJECT_HPP_

#include "../LSValue.hpp"

namespace ls {

class LSClass;

class LSObject : public LSValue {
public:
	static LSObject* constructor();

	std::map<std::string, LSValue*> values;
	LSClass* clazz;
	bool readonly;

	LSObject();
	LSObject(LSClass*);
	virtual ~LSObject();

	/** LSObject methods **/
	void addField(const char* name, LSValue* value);
	LSValue* getField(std::string name);
	LSArray<LSValue*>* ls_get_keys() const;
	LSArray<LSValue*>* ls_get_values() const;
	template <class F>
	LSObject* ls_map(F fun) const;

	/** LSValue methods **/
	bool to_bool() const override;
	bool ls_not() const override;
	bool eq(const LSValue*) const override;
	bool lt(const LSValue*) const override;
	bool in(const LSValue*) const override;
	LSValue* attr(VM* vm, const std::string& key) const override;
	LSValue** attrL(const std::string& key) override;
	int abso() const override;
	LSValue* clone() const override;
	std::ostream& dump(std::ostream& os, int level) const override;
	std::string json() const override;
	LSValue* getClass(VM* vm) const override;
};

}

#endif
