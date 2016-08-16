#include "Type.hpp"

using namespace std;

namespace ls {

const BaseRawType* const RawType::UNKNOWN = new BaseRawType();
const VoidRawType* const RawType::VOID = new VoidRawType();
const NullRawType* const RawType::NULLL = new NullRawType();
const BooleanRawType* const RawType::BOOLEAN = new BooleanRawType();
const NumberRawType* const RawType::NUMBER = new NumberRawType();
const IntegerRawType* const RawType::INTEGER = new IntegerRawType();
const LongRawType* const RawType::LONG = new LongRawType();
const FloatRawType* const RawType::FLOAT = new FloatRawType();
const StringRawType* const RawType::STRING = new StringRawType();
const ArrayRawType* const RawType::ARRAY = new ArrayRawType();
const MapRawType* const RawType::MAP = new MapRawType();
const IntervalRawType* const RawType::INTERVAL = new IntervalRawType();
const ObjectRawType* const RawType::OBJECT = new ObjectRawType();
const FunctionRawType* const RawType::FUNCTION = new FunctionRawType();
const ClassRawType* const RawType::CLASS = new ClassRawType();

const Type Type::UNKNOWN(RawType::UNKNOWN, Nature::UNKNOWN);

const Type Type::VOID(RawType::VOID, Nature::VOID);
const Type Type::VALUE(RawType::UNKNOWN, Nature::VALUE);
const Type Type::POINTER(RawType::UNKNOWN, Nature::POINTER);

const Type Type::NULLL(RawType::NULLL, Nature::POINTER);
const Type Type::BOOLEAN(RawType::BOOLEAN, Nature::VALUE);
const Type Type::BOOLEAN_P(RawType::BOOLEAN, Nature::POINTER);
const Type Type::NUMBER(RawType::NUMBER, Nature::POINTER);
const Type Type::INTEGER(RawType::INTEGER, Nature::VALUE);
const Type Type::INTEGER_P(RawType::INTEGER, Nature::POINTER);
const Type Type::LONG(RawType::LONG, Nature::VALUE);
const Type Type::FLOAT(RawType::FLOAT, Nature::VALUE);
const Type Type::FLOAT_P(RawType::FLOAT, Nature::POINTER);
const Type Type::STRING(RawType::STRING, Nature::POINTER);
const Type Type::OBJECT(RawType::OBJECT, Nature::POINTER);
const Type Type::ARRAY(RawType::ARRAY, Nature::POINTER);
const Type Type::PTR_ARRAY(RawType::ARRAY, Nature::POINTER, Type::POINTER);
const Type Type::INT_ARRAY(RawType::ARRAY, Nature::POINTER, Type::INTEGER);
const Type Type::FLOAT_ARRAY(RawType::ARRAY, Nature::POINTER, Type::FLOAT);
const Type Type::STRING_ARRAY(RawType::ARRAY, Nature::POINTER, Type::STRING);
const Type Type::MAP(RawType::MAP, Nature::POINTER, {Type::UNKNOWN, Type::UNKNOWN});
const Type Type::PTR_PTR_MAP(RawType::MAP, Nature::POINTER, {Type::POINTER, Type::POINTER});
const Type Type::PTR_INT_MAP(RawType::MAP, Nature::POINTER, {Type::POINTER, Type::INTEGER});
const Type Type::PTR_FLOAT_MAP(RawType::MAP, Nature::POINTER, {Type::POINTER, Type::FLOAT});
const Type Type::INT_PTR_MAP(RawType::MAP, Nature::POINTER, {Type::INTEGER, Type::POINTER});
const Type Type::INT_INT_MAP(RawType::MAP, Nature::POINTER, {Type::INTEGER, Type::INTEGER});
const Type Type::INT_FLOAT_MAP(RawType::MAP, Nature::POINTER, {Type::INTEGER, Type::FLOAT});
const Type Type::INTERVAL(RawType::INTERVAL, Nature::POINTER, Type::INTEGER);

const Type Type::FUNCTION(RawType::FUNCTION, Nature::VALUE);
const Type Type::FUNCTION_P(RawType::FUNCTION, Nature::POINTER);
const Type Type::CLASS(RawType::CLASS, Nature::POINTER);

Type::Type() {
	raw_type = RawType::UNKNOWN;
	nature = Nature::UNKNOWN;
	native = false;
	clazz = "?";
}

Type::Type(const BaseRawType* raw_type, Nature nature, bool native) {
	this->raw_type = raw_type;
	this->nature = nature;
	this->native = native;
	this->clazz = raw_type->getClass();
}

Type::Type(const BaseRawType* raw_type, Nature nature, const Type& elements_type, bool native) {
	this->raw_type = raw_type;
	this->nature = nature;
	this->native = native;
	this->clazz = raw_type->getClass();
	this->setElementType(elements_type);
}

Type::Type(const BaseRawType* raw_type, Nature nature, const vector<Type>& element_type, bool native) {
	this->raw_type = raw_type;
	this->nature = nature;
	this->native = native;
	this->clazz = raw_type->getClass();
	this->element_types = element_type;
}

bool Type::must_manage_memory() const {
	return nature == Nature::POINTER and not native;
}

Type Type::getReturnType() const {
	if (return_types.size() == 0) {
		return Type::UNKNOWN;
	}
	return return_types[0];
}

void Type::setReturnType(Type type) {
	if (return_types.size() == 0) {
		return_types.push_back(Type::UNKNOWN);
	}
	return_types[0] = type;
}

void Type::addArgumentType(Type type) {
	arguments_types.push_back(type);
}

void Type::setArgumentType(size_t index, Type type) {
	while (arguments_types.size() <= index) {
		arguments_types.push_back(Type::UNKNOWN);
	}
	arguments_types[index] = type;
}

/*
 * By default, all arguments are type INTEGER, but if we see it's not always
 * a integer, it will switch to UNKNOWN
 */
const Type Type::getArgumentType(size_t index) const {
	if (index >= arguments_types.size()) {
		return Type::UNKNOWN;
	}
	return arguments_types[index];
}

const std::vector<Type>& Type::getArgumentTypes() const {
	return arguments_types;
}

const Type Type::getElementType(size_t i) const {
	if (i < element_types.size()) {
		return element_types[i];
	}
	return Type::UNKNOWN;
}

void Type::setElementType(Type type) {
	if (element_types.size() == 0) {
		element_types.push_back(type);
	} else {
		element_types[0] = type;
	}
}

bool Type::will_take(const vector<Type>& args_type) {

	bool changed = false;

	for (size_t i = 0; i < args_type.size(); ++i) {
		Type current_type = getArgumentType(i);

		/*
		if (current_type.nature == Nature::MIXED) {
			return; // No change, it is still mixed
		}
		*/

		if (current_type.nature == Nature::UNKNOWN) {
			setArgumentType(i, args_type[i]);
			changed = true;
		} else {
			if (current_type.nature == Nature::VALUE and args_type[i].nature == Nature::POINTER) {
				setArgumentType(i, Type(RawType::UNKNOWN, Nature::POINTER));
				changed = true;
			}
		}
	}
	return changed;
}

bool Type::will_take_element(const Type& element_type) {

	if (raw_type != RawType::ARRAY) {
		return false;
	}

	Type current = getElementType();

	if (current == element_type) {
		return false;
	}

	setElementType(element_type);
	return true;
}

Type Type::mix(const Type& x) const {

	Type type(*this);
	if (x.raw_type == RawType::FLOAT) type.raw_type = RawType::FLOAT;
	if (x.nature == Nature::POINTER) type.nature = Nature::POINTER;
	return type;
}

void Type::toJson(ostream& os) const {
	os << "{\"type\":\"" << raw_type->getJsonName() << "\"";

	if (raw_type == RawType::FUNCTION) {
		os << ",\"args\":[";
		for (unsigned t = 0; t < arguments_types.size(); ++t) {
			if (t > 0) os << ",";
			arguments_types[t].toJson(os);
		}
		os << "]";
		os << ",\"return\":";
		getReturnType().toJson(os);
	}
	os << "}";
}

bool Type::isNumber() const {
	return dynamic_cast<const NumberRawType*>(raw_type) != nullptr;
}

bool Type::operator ==(const Type& type) const {
	return raw_type == type.raw_type &&
			nature == type.nature &&
			native == type.native &&
			clazz == type.clazz &&
			element_types == type.element_types &&
			return_types == type.return_types &&
			arguments_types == type.arguments_types;
}

bool Type::compatible(const Type& type) const {
/*  Can we convert type into this ?
 *   {float}.compatible({int}) == true
 *   {int*}.compatible({int}) == true
 */

	if (this->nature == Nature::VALUE && type.nature == Nature::POINTER) {
		return false;
	}

	if (this->raw_type != type.raw_type) {

		// Every type is compatible with 'Unknown' type
		if (this->raw_type == RawType::UNKNOWN) {
			return true;
		}

		// 'Integer' is compatible with 'Float'
		if (this->raw_type == RawType::FLOAT and type.raw_type == RawType::INTEGER) {
			return true;
		}

		// All numbers types are compatible with the base 'Number' type
		if (this->raw_type == RawType::NUMBER and (
			type.raw_type == RawType::INTEGER or
			type.raw_type == RawType::LONG or
			type.raw_type == RawType::FLOAT
		)) return true;

		return false;
	}

	if (this->raw_type == RawType::ARRAY) {
		return this->getElementType().compatible(type.getElementType());
	}

	if (this->raw_type == RawType::MAP) {
		return getElementType(0).compatible(type.getElementType(0))
				&& getElementType(1).compatible(type.getElementType(1));
	}

	return true;
}

bool Type::list_compatible(const std::vector<Type>& expected, const std::vector<Type>& actual) {

	if (expected.size() != actual.size()) return false;

	for (size_t i = 0; i < expected.size(); ++i) {
		// Can we convert type actual[i] into type expected[i] ?
		if (not expected[i].compatible(actual[i])) return false;
	}
	return true;
}

bool Type::list_more_specific(const std::vector<Type>& old, const std::vector<Type>& neww) {

	if (old.size() != neww.size()) return false;

	for (size_t i = 0; i < old.size(); ++i) {
		if (Type::more_specific(old[i], neww[i])) {
			return true;
		}
	}
	return false;
}

bool Type::more_specific(const Type& old, const Type& neww) {

	if (neww.raw_type != old.raw_type) {
		if (old.raw_type == RawType::UNKNOWN) {
			return true;
		}
		if (old.raw_type == RawType::NUMBER
				&& (neww.raw_type == RawType::INTEGER || neww.raw_type == RawType::LONG || neww.raw_type == RawType::FLOAT)) {
			return true;
		}
		if (old.raw_type == RawType::FLOAT
				&& (neww.raw_type == RawType::INTEGER || neww.raw_type == RawType::LONG)) {
			return true;
		}
		if (old.raw_type == RawType::LONG
				&& neww.raw_type == RawType::INTEGER) {
			return true;
		}
	}

	if (neww.raw_type == RawType::ARRAY and old.raw_type == RawType::ARRAY) {
		if (Type::more_specific(old.getElementType(), neww.getElementType())) {
			return true;
		}
	}

	if (neww.raw_type == RawType::MAP and old.raw_type == RawType::MAP) {
		if (Type::more_specific(old.getElementType(0), neww.getElementType(0))
				|| Type::more_specific(old.getElementType(1), neww.getElementType(1))) {
			return true;
		}
	}

	if (neww.raw_type == RawType::FUNCTION and old.raw_type == RawType::FUNCTION) {
		if (Type::more_specific(old.getArgumentType(0), neww.getArgumentType(0))) { //! TODO only the first arg
			return true;
		}
	}
	return false;
}

Type Type::get_compatible_type(const Type& t1, const Type& t2) {

	if (t1 == t2) {
		return t1;
	}

	if (t1.nature == Nature::POINTER and t2.nature == Nature::VALUE) {
		if (t1.raw_type == t2.raw_type) {
			if (t1.element_types == t2.element_types
					&& t1.return_types == t2.return_types
					&& t1.arguments_types == t2.arguments_types) {
				return t1; // They are identical except the Nature
			}
			return Type(t1.raw_type, Nature::POINTER); // They have the same raw_type : for example {function* ({int})->{int}} and {function ({int})->{void}}
		}
		return Type::POINTER;
	}

	// symmetric of last it statement
	if (t2.nature == Nature::POINTER and t1.nature == Nature::VALUE) {
		if (t2.raw_type == t1.raw_type) {
			if (t2.element_types == t1.element_types
					&& t2.return_types == t1.return_types
					&& t2.arguments_types == t1.arguments_types) {
				return t2;
			}
			return Type(t2.raw_type, Nature::POINTER);
		}
		return Type::POINTER;
	}

	if (t1.raw_type == RawType::UNKNOWN) {
		return t2;
	}
	if (t2.raw_type == RawType::UNKNOWN) {
		return t1;
	}

	if (t1.compatible(t2)) {
		return t1;
	}
	if (t2.compatible(t1)) {
		return t2;
	}
	return Type::POINTER;
}

string Type::get_nature_name(const Nature& nature) {
	switch (nature) {
	case Nature::POINTER:
		return "POINTER";
	case Nature::UNKNOWN:
		return "UNKNOWN";
	case Nature::VALUE:
		return "VALUE";
	case Nature::VOID:
		return "VOID";
	default:
		return "??";
	}
}

string Type::get_nature_symbol(const Nature& nature) {
	switch (nature) {
	case Nature::POINTER:
		return "*";
	case Nature::UNKNOWN:
		return "?";
	case Nature::VALUE:
		return "";
	case Nature::VOID:
		return "void";
	default:
		return "???";
	}
}

ostream& operator << (ostream& os, const Type& type) {

	if (type == Type::VOID) {
		os << "{void}";
		return os;
	}

	os << "{" << type.raw_type->getName() << Type::get_nature_symbol(type.nature);

	if (type.raw_type == RawType::FUNCTION) {
		os << " (";
		for (unsigned t = 0; t < type.arguments_types.size(); ++t) {
			if (t > 0) os << ", ";
			os << type.arguments_types[t];
		}
		os << ") → " << type.getReturnType();
	}
	if (type.raw_type == RawType::ARRAY) {
		os << " of " << type.getElementType();
	}
	if (type.raw_type == RawType::MAP) {
		os << " of " << type.getElementType(0) << " → " << type.getElementType(1);
	}
	os << "}";
	return os;
}

}
