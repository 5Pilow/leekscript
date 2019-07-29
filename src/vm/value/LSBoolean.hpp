#ifndef LSBOOLEAN_HPP_
#define LSBOOLEAN_HPP_

#include <string>
#include "../LSValue.hpp"

namespace ls {

class LSBoolean : public LSValue {
private:

	LSBoolean(bool value);

public:

	const bool value;

	static LSBoolean* false_val;
	static LSBoolean* true_val;
	static LSBoolean* create(bool value) {
		return new LSBoolean(value);
	}
	static LSBoolean* get(bool value) {
		return value ? true_val : false_val;
	}
	static void set_true_value(LSBoolean* v) {
		true_val = v;
	}
	static void set_false_value(LSBoolean* v) {
		false_val = v;
	}

	virtual ~LSBoolean();

	bool to_bool() const override;
	bool ls_not() const override;
	LSValue* ls_tilde() override;
	LSValue* ls_minus() override;

	LSValue* add(LSValue* v) override;
	LSValue* sub(LSValue* v) override;
	bool eq(const LSValue*) const override;
	bool lt(const LSValue*) const override;
	bool operator < (int value) const override;
	bool operator < (double value) const override;

	int abso() const override;

	LSValue* clone() const override;

	std::ostream& dump(std::ostream& os, int level) const override;
	std::string json() const override;

	LSValue* getClass(VM* vm) const override;
};

}

#endif
