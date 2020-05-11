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
	static void std_add_field(LSObject* object, const char* name, LSValue* value);
	LSValue* getField(std::string name);
	static LSArray<LSValue*>* ls_get_keys(const LSObject* const object);
	static LSArray<LSValue*>* ls_get_values(const LSObject* const object);
	template <class F>
	static LSObject* ls_map(const LSObject* const object, F fun);
	static bool ls_in(const LSObject* const object, const LSValue* value);

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
