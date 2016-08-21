#include <iostream>
#include "LSValue.hpp"
#include "value/LSNumber.hpp"
#include "value/LSNull.hpp"
#include "value/LSBoolean.hpp"
#include "value/LSArray.hpp"
#include "value/LSMap.hpp"
#include "value/LSSet.hpp"
#include "value/LSObject.hpp"
#include "value/LSFunction.hpp"
#include "VM.hpp"

using namespace std;

namespace ls {

int LSValue::obj_count = 0;
int LSValue::obj_deleted = 0;
#if DEBUG > 1
	extern std::map<LSValue*, LSValue*> objs;
#endif

LSValue::LSValue() : refs(0) {
	obj_count++;
	#if DEBUG > 1
		objs.insert({this, this});
	#endif
}

LSValue::LSValue(const LSValue& ) : refs(0) {
	obj_count++;
	#if DEBUG > 1
		objs.insert({this, this});
	#endif
}

LSValue::~LSValue() {
	obj_deleted++;
	#if DEBUG > 1
		objs.erase(this);
	#endif
}

LSValue* LSValue::operator - () const { return LSNull::get(); }
LSValue* LSValue::operator ! () const { return LSNull::get(); }
LSValue* LSValue::operator ~ () const { return LSNull::get(); }

LSValue* LSValue::operator ++ () { return LSNull::get(); }
LSValue* LSValue::operator ++ (int) { return LSNull::get(); }

LSValue* LSValue::operator -- () { return LSNull::get(); }
LSValue* LSValue::operator -- (int) { return LSNull::get(); }

LSValue* LSValue::operator + (const LSNull*) const { return LSNull::get(); }
LSValue* LSValue::operator + (const LSBoolean*) const { return LSNull::get(); }
LSValue* LSValue::operator + (const LSNumber*) const { return LSNull::get(); }
LSValue* LSValue::operator + (const LSString*) const { return LSNull::get(); }
LSValue* LSValue::operator + (const LSArray<LSValue*>*) const { return LSNull::get(); }
LSValue* LSValue::operator + (const LSArray<int>*) const { return LSNull::get(); }
LSValue* LSValue::operator + (const LSArray<double>*) const { return LSNull::get(); }
LSValue* LSValue::operator + (const LSMap<LSValue*,LSValue*>*) const { return LSNull::get(); }
LSValue* LSValue::operator + (const LSMap<LSValue*,int>*) const { return LSNull::get(); }
LSValue* LSValue::operator + (const LSMap<LSValue*,double>*) const { return LSNull::get(); }
LSValue* LSValue::operator + (const LSMap<int,LSValue*>*) const { return LSNull::get(); }
LSValue* LSValue::operator + (const LSMap<int,int>*) const { return LSNull::get(); }
LSValue* LSValue::operator + (const LSMap<int,double>*) const { return LSNull::get(); }
LSValue* LSValue::operator + (const LSSet<LSValue*>*) const { return LSNull::get(); }
LSValue* LSValue::operator + (const LSSet<int>*) const { return LSNull::get(); }
LSValue* LSValue::operator + (const LSSet<double>*) const { return LSNull::get(); }
LSValue* LSValue::operator + (const LSObject*) const { return LSNull::get(); }
LSValue* LSValue::operator + (const LSFunction*) const { return LSNull::get(); }
LSValue* LSValue::operator + (const LSClass*) const { return LSNull::get(); }

LSValue* LSValue::operator += (const LSNull*) { return LSNull::get(); }
LSValue* LSValue::operator += (const LSBoolean*) { return LSNull::get(); }
LSValue* LSValue::operator += (const LSNumber*) { return LSNull::get(); }
LSValue* LSValue::operator += (const LSString*) { return LSNull::get(); }
LSValue* LSValue::operator += (const LSArray<LSValue*>*) { return LSNull::get(); }
LSValue* LSValue::operator += (const LSArray<int>*) { return LSNull::get(); }
LSValue* LSValue::operator += (const LSArray<double>*) { return LSNull::get(); }
LSValue* LSValue::operator += (const LSMap<LSValue*,LSValue*>*) { return LSNull::get(); }
LSValue* LSValue::operator += (const LSMap<LSValue*,int>*) { return LSNull::get(); }
LSValue* LSValue::operator += (const LSMap<LSValue*,double>*) { return LSNull::get(); }
LSValue* LSValue::operator += (const LSMap<int,LSValue*>*) { return LSNull::get(); }
LSValue* LSValue::operator += (const LSMap<int,int>*) { return LSNull::get(); }
LSValue* LSValue::operator += (const LSMap<int,double>*) { return LSNull::get(); }
LSValue* LSValue::operator += (const LSSet<LSValue*>*) { return LSNull::get(); }
LSValue* LSValue::operator += (const LSSet<int>*) { return LSNull::get(); }
LSValue* LSValue::operator += (const LSSet<double>*) { return LSNull::get(); }
LSValue* LSValue::operator += (const LSObject*) { return LSNull::get(); }
LSValue* LSValue::operator += (const LSFunction*) { return LSNull::get(); }
LSValue* LSValue::operator += (const LSClass*) { return LSNull::get(); }

LSValue* LSValue::operator - (const LSNull*) const { return LSNull::get(); }
LSValue* LSValue::operator - (const LSBoolean*) const { return LSNull::get(); }
LSValue* LSValue::operator - (const LSNumber*) const { return LSNull::get(); }
LSValue* LSValue::operator - (const LSString*) const { return LSNull::get(); }
LSValue* LSValue::operator - (const LSArray<LSValue*>*) const { return LSNull::get(); }
LSValue* LSValue::operator - (const LSArray<int>*) const { return LSNull::get(); }
LSValue* LSValue::operator - (const LSArray<double>*) const { return LSNull::get(); }
LSValue* LSValue::operator - (const LSMap<LSValue*,LSValue*>*) const { return LSNull::get(); }
LSValue* LSValue::operator - (const LSMap<LSValue*,int>*) const { return LSNull::get(); }
LSValue* LSValue::operator - (const LSMap<LSValue*,double>*) const { return LSNull::get(); }
LSValue* LSValue::operator - (const LSMap<int,LSValue*>*) const { return LSNull::get(); }
LSValue* LSValue::operator - (const LSMap<int,int>*) const { return LSNull::get(); }
LSValue* LSValue::operator - (const LSMap<int,double>*) const { return LSNull::get(); }
LSValue* LSValue::operator - (const LSSet<LSValue*>*) const { return LSNull::get(); }
LSValue* LSValue::operator - (const LSSet<int>*) const { return LSNull::get(); }
LSValue* LSValue::operator - (const LSSet<double>*) const { return LSNull::get(); }
LSValue* LSValue::operator - (const LSObject*) const { return LSNull::get(); }
LSValue* LSValue::operator - (const LSFunction*) const { return LSNull::get(); }
LSValue* LSValue::operator - (const LSClass*) const { return LSNull::get(); }

LSValue* LSValue::operator -= (const LSNull*) { return LSNull::get(); }
LSValue* LSValue::operator -= (const LSBoolean*) { return LSNull::get(); }
LSValue* LSValue::operator -= (const LSNumber*) { return LSNull::get(); }
LSValue* LSValue::operator -= (const LSString*) { return LSNull::get(); }
LSValue* LSValue::operator -= (const LSArray<LSValue*>*) { return LSNull::get(); }
LSValue* LSValue::operator -= (const LSArray<int>*) { return LSNull::get(); }
LSValue* LSValue::operator -= (const LSArray<double>*) { return LSNull::get(); }
LSValue* LSValue::operator -= (const LSMap<LSValue*,LSValue*>*) { return LSNull::get(); }
LSValue* LSValue::operator -= (const LSMap<LSValue*,int>*) { return LSNull::get(); }
LSValue* LSValue::operator -= (const LSMap<LSValue*,double>*) { return LSNull::get(); }
LSValue* LSValue::operator -= (const LSMap<int,LSValue*>*) { return LSNull::get(); }
LSValue* LSValue::operator -= (const LSMap<int,int>*) { return LSNull::get(); }
LSValue* LSValue::operator -= (const LSMap<int,double>*) { return LSNull::get(); }
LSValue* LSValue::operator -= (const LSSet<LSValue*>*) { return LSNull::get(); }
LSValue* LSValue::operator -= (const LSSet<int>*) { return LSNull::get(); }
LSValue* LSValue::operator -= (const LSSet<double>*) { return LSNull::get(); }
LSValue* LSValue::operator -= (const LSObject*) { return LSNull::get(); }
LSValue* LSValue::operator -= (const LSFunction*) { return LSNull::get(); }
LSValue* LSValue::operator -= (const LSClass*) { return LSNull::get(); }

LSValue* LSValue::operator * (const LSNull*) const { return LSNull::get(); }
LSValue* LSValue::operator * (const LSBoolean*) const { return LSNull::get(); }
LSValue* LSValue::operator * (const LSNumber*) const { return LSNull::get(); }
LSValue* LSValue::operator * (const LSString*) const { return LSNull::get(); }
LSValue* LSValue::operator * (const LSArray<LSValue*>*) const { return LSNull::get(); }
LSValue* LSValue::operator * (const LSArray<int>*) const { return LSNull::get(); }
LSValue* LSValue::operator * (const LSArray<double>*) const { return LSNull::get(); }
LSValue* LSValue::operator * (const LSMap<LSValue*,LSValue*>*) const { return LSNull::get(); }
LSValue* LSValue::operator * (const LSMap<LSValue*,int>*) const { return LSNull::get(); }
LSValue* LSValue::operator * (const LSMap<LSValue*,double>*) const { return LSNull::get(); }
LSValue* LSValue::operator * (const LSMap<int,LSValue*>*) const { return LSNull::get(); }
LSValue* LSValue::operator * (const LSMap<int,int>*) const { return LSNull::get(); }
LSValue* LSValue::operator * (const LSMap<int,double>*) const { return LSNull::get(); }
LSValue* LSValue::operator * (const LSSet<LSValue*>*) const { return LSNull::get(); }
LSValue* LSValue::operator * (const LSSet<int>*) const { return LSNull::get(); }
LSValue* LSValue::operator * (const LSSet<double>*) const { return LSNull::get(); }
LSValue* LSValue::operator * (const LSObject*) const { return LSNull::get(); }
LSValue* LSValue::operator * (const LSFunction*) const { return LSNull::get(); }
LSValue* LSValue::operator * (const LSClass*) const { return LSNull::get(); }

LSValue* LSValue::operator *= (const LSNull*) { return LSNull::get(); }
LSValue* LSValue::operator *= (const LSBoolean*) { return LSNull::get(); }
LSValue* LSValue::operator *= (const LSNumber*) { return LSNull::get(); }
LSValue* LSValue::operator *= (const LSString*) { return LSNull::get(); }
LSValue* LSValue::operator *= (const LSArray<LSValue*>*) { return LSNull::get(); }
LSValue* LSValue::operator *= (const LSArray<int>*) { return LSNull::get(); }
LSValue* LSValue::operator *= (const LSArray<double>*) { return LSNull::get(); }
LSValue* LSValue::operator *= (const LSMap<LSValue*,LSValue*>*) { return LSNull::get(); }
LSValue* LSValue::operator *= (const LSMap<LSValue*,int>*) { return LSNull::get(); }
LSValue* LSValue::operator *= (const LSMap<LSValue*,double>*) { return LSNull::get(); }
LSValue* LSValue::operator *= (const LSMap<int,LSValue*>*) { return LSNull::get(); }
LSValue* LSValue::operator *= (const LSMap<int,int>*) { return LSNull::get(); }
LSValue* LSValue::operator *= (const LSMap<int,double>*) { return LSNull::get(); }
LSValue* LSValue::operator *= (const LSSet<LSValue*>*) { return LSNull::get(); }
LSValue* LSValue::operator *= (const LSSet<int>*) { return LSNull::get(); }
LSValue* LSValue::operator *= (const LSSet<double>*) { return LSNull::get(); }
LSValue* LSValue::operator *= (const LSObject*) { return LSNull::get(); }
LSValue* LSValue::operator *= (const LSFunction*) { return LSNull::get(); }
LSValue* LSValue::operator *= (const LSClass*) { return LSNull::get(); }

LSValue* LSValue::operator / (const LSNull*) const { return LSNull::get(); }
LSValue* LSValue::operator / (const LSBoolean*) const { return LSNull::get(); }
LSValue* LSValue::operator / (const LSNumber*) const { return LSNull::get(); }
LSValue* LSValue::operator / (const LSString*) const { return LSNull::get(); }
LSValue* LSValue::operator / (const LSArray<LSValue*>*) const { return LSNull::get(); }
LSValue* LSValue::operator / (const LSArray<int>*) const { return LSNull::get(); }
LSValue* LSValue::operator / (const LSArray<double>*) const { return LSNull::get(); }
LSValue* LSValue::operator / (const LSMap<LSValue*,LSValue*>*) const { return LSNull::get(); }
LSValue* LSValue::operator / (const LSMap<LSValue*,int>*) const { return LSNull::get(); }
LSValue* LSValue::operator / (const LSMap<LSValue*,double>*) const { return LSNull::get(); }
LSValue* LSValue::operator / (const LSMap<int,LSValue*>*) const { return LSNull::get(); }
LSValue* LSValue::operator / (const LSMap<int,int>*) const { return LSNull::get(); }
LSValue* LSValue::operator / (const LSMap<int,double>*) const { return LSNull::get(); }
LSValue* LSValue::operator / (const LSSet<LSValue*>*) const { return LSNull::get(); }
LSValue* LSValue::operator / (const LSSet<int>*) const { return LSNull::get(); }
LSValue* LSValue::operator / (const LSSet<double>*) const { return LSNull::get(); }
LSValue* LSValue::operator / (const LSObject*) const { return LSNull::get(); }
LSValue* LSValue::operator / (const LSFunction*) const { return LSNull::get(); }
LSValue* LSValue::operator / (const LSClass*) const { return LSNull::get(); }

LSValue* LSValue::operator /= (const LSNull*) { return LSNull::get(); }
LSValue* LSValue::operator /= (const LSBoolean*) { return LSNull::get(); }
LSValue* LSValue::operator /= (const LSNumber*) { return LSNull::get(); }
LSValue* LSValue::operator /= (const LSString*) { return LSNull::get(); }
LSValue* LSValue::operator /= (const LSArray<LSValue*>*) { return LSNull::get(); }
LSValue* LSValue::operator /= (const LSArray<int>*) { return LSNull::get(); }
LSValue* LSValue::operator /= (const LSArray<double>*) { return LSNull::get(); }
LSValue* LSValue::operator /= (const LSMap<LSValue*,LSValue*>*) { return LSNull::get(); }
LSValue* LSValue::operator /= (const LSMap<LSValue*,int>*) { return LSNull::get(); }
LSValue* LSValue::operator /= (const LSMap<LSValue*,double>*) { return LSNull::get(); }
LSValue* LSValue::operator /= (const LSMap<int,LSValue*>*) { return LSNull::get(); }
LSValue* LSValue::operator /= (const LSMap<int,int>*) { return LSNull::get(); }
LSValue* LSValue::operator /= (const LSMap<int,double>*) { return LSNull::get(); }
LSValue* LSValue::operator /= (const LSSet<LSValue*>*) { return LSNull::get(); }
LSValue* LSValue::operator /= (const LSSet<int>*) { return LSNull::get(); }
LSValue* LSValue::operator /= (const LSSet<double>*) { return LSNull::get(); }
LSValue* LSValue::operator /= (const LSObject*) { return LSNull::get(); }
LSValue* LSValue::operator /= (const LSFunction*) { return LSNull::get(); }
LSValue* LSValue::operator /= (const LSClass*) { return LSNull::get(); }

LSValue* LSValue::poww(const LSNull*) const { return LSNull::get(); }
LSValue* LSValue::poww(const LSBoolean*) const { return LSNull::get(); }
LSValue* LSValue::poww(const LSNumber*) const { return LSNull::get(); }
LSValue* LSValue::poww(const LSString*) const { return LSNull::get(); }
LSValue* LSValue::poww(const LSArray<LSValue*>*) const { return LSNull::get(); }
LSValue* LSValue::poww(const LSArray<int>*) const { return LSNull::get(); }
LSValue* LSValue::poww(const LSArray<double>*) const { return LSNull::get(); }
LSValue* LSValue::poww(const LSMap<LSValue*,LSValue*>*) const { return LSNull::get(); }
LSValue* LSValue::poww(const LSMap<LSValue*,int>*) const { return LSNull::get(); }
LSValue* LSValue::poww(const LSMap<LSValue*,double>*) const { return LSNull::get(); }
LSValue* LSValue::poww(const LSMap<int,LSValue*>*) const { return LSNull::get(); }
LSValue* LSValue::poww(const LSMap<int,int>*) const { return LSNull::get(); }
LSValue* LSValue::poww(const LSMap<int,double>*) const { return LSNull::get(); }
LSValue* LSValue::poww(const LSSet<LSValue*>*) const { return LSNull::get(); }
LSValue* LSValue::poww(const LSSet<int>*) const { return LSNull::get(); }
LSValue* LSValue::poww(const LSSet<double>*) const { return LSNull::get(); }
LSValue* LSValue::poww(const LSObject*) const { return LSNull::get(); }
LSValue* LSValue::poww(const LSFunction*) const { return LSNull::get(); }
LSValue* LSValue::poww(const LSClass*) const { return LSNull::get(); }

LSValue* LSValue::pow_eq(const LSNull*) { return LSNull::get(); }
LSValue* LSValue::pow_eq(const LSBoolean*) { return LSNull::get(); }
LSValue* LSValue::pow_eq(const LSNumber*) { return LSNull::get(); }
LSValue* LSValue::pow_eq(const LSString*) { return LSNull::get(); }
LSValue* LSValue::pow_eq(const LSArray<LSValue*>*) { return LSNull::get(); }
LSValue* LSValue::pow_eq(const LSArray<int>*) { return LSNull::get(); }
LSValue* LSValue::pow_eq(const LSArray<double>*) { return LSNull::get(); }
LSValue* LSValue::pow_eq(const LSMap<LSValue*,LSValue*>*) { return LSNull::get(); }
LSValue* LSValue::pow_eq(const LSMap<LSValue*,int>*) { return LSNull::get(); }
LSValue* LSValue::pow_eq(const LSMap<LSValue*,double>*) { return LSNull::get(); }
LSValue* LSValue::pow_eq(const LSMap<int,LSValue*>*) { return LSNull::get(); }
LSValue* LSValue::pow_eq(const LSMap<int,int>*) { return LSNull::get(); }
LSValue* LSValue::pow_eq(const LSMap<int,double>*) { return LSNull::get(); }
LSValue* LSValue::pow_eq(const LSSet<LSValue*>*) { return LSNull::get(); }
LSValue* LSValue::pow_eq(const LSSet<int>*) { return LSNull::get(); }
LSValue* LSValue::pow_eq(const LSSet<double>*) { return LSNull::get(); }
LSValue* LSValue::pow_eq(const LSObject*) { return LSNull::get(); }
LSValue* LSValue::pow_eq(const LSFunction*) { return LSNull::get(); }
LSValue* LSValue::pow_eq(const LSClass*) { return LSNull::get(); }

LSValue* LSValue::operator % (const LSNull*) const { return LSNull::get(); }
LSValue* LSValue::operator % (const LSBoolean*) const { return LSNull::get(); }
LSValue* LSValue::operator % (const LSNumber*) const { return LSNull::get(); }
LSValue* LSValue::operator % (const LSString*) const { return LSNull::get(); }
LSValue* LSValue::operator % (const LSArray<LSValue*>*) const { return LSNull::get(); }
LSValue* LSValue::operator % (const LSArray<int>*) const { return LSNull::get(); }
LSValue* LSValue::operator % (const LSArray<double>*) const { return LSNull::get(); }
LSValue* LSValue::operator % (const LSMap<LSValue*,LSValue*>*) const { return LSNull::get(); }
LSValue* LSValue::operator % (const LSMap<LSValue*,int>*) const { return LSNull::get(); }
LSValue* LSValue::operator % (const LSMap<LSValue*,double>*) const { return LSNull::get(); }
LSValue* LSValue::operator % (const LSMap<int,LSValue*>*) const { return LSNull::get(); }
LSValue* LSValue::operator % (const LSMap<int,int>*) const { return LSNull::get(); }
LSValue* LSValue::operator % (const LSMap<int,double>*) const { return LSNull::get(); }
LSValue* LSValue::operator % (const LSSet<LSValue*>*) const { return LSNull::get(); }
LSValue* LSValue::operator % (const LSSet<int>*) const { return LSNull::get(); }
LSValue* LSValue::operator % (const LSSet<double>*) const { return LSNull::get(); }
LSValue* LSValue::operator % (const LSObject*) const { return LSNull::get(); }
LSValue* LSValue::operator % (const LSFunction*) const { return LSNull::get(); }
LSValue* LSValue::operator % (const LSClass*) const { return LSNull::get(); }

LSValue* LSValue::operator %= (const LSNull*) { return LSNull::get(); }
LSValue* LSValue::operator %= (const LSBoolean*) { return LSNull::get(); }
LSValue* LSValue::operator %= (const LSNumber*) { return LSNull::get(); }
LSValue* LSValue::operator %= (const LSString*) { return LSNull::get(); }
LSValue* LSValue::operator %= (const LSArray<LSValue*>*) { return LSNull::get(); }
LSValue* LSValue::operator %= (const LSArray<int>*) { return LSNull::get(); }
LSValue* LSValue::operator %= (const LSArray<double>*) { return LSNull::get(); }
LSValue* LSValue::operator %= (const LSMap<LSValue*,LSValue*>*) { return LSNull::get(); }
LSValue* LSValue::operator %= (const LSMap<LSValue*,int>*) { return LSNull::get(); }
LSValue* LSValue::operator %= (const LSMap<LSValue*,double>*) { return LSNull::get(); }
LSValue* LSValue::operator %= (const LSMap<int,LSValue*>*) { return LSNull::get(); }
LSValue* LSValue::operator %= (const LSMap<int,int>*) { return LSNull::get(); }
LSValue* LSValue::operator %= (const LSMap<int,double>*) { return LSNull::get(); }
LSValue* LSValue::operator %= (const LSSet<LSValue*>*) { return LSNull::get(); }
LSValue* LSValue::operator %= (const LSSet<int>*) { return LSNull::get(); }
LSValue* LSValue::operator %= (const LSSet<double>*) { return LSNull::get(); }
LSValue* LSValue::operator %= (const LSObject*) { return LSNull::get(); }
LSValue* LSValue::operator %= (const LSFunction*) { return LSNull::get(); }
LSValue* LSValue::operator %= (const LSClass*) { return LSNull::get(); }

bool LSValue::operator == (const LSNull*) const { return false; }
bool LSValue::operator == (const LSBoolean*) const { return false; }
bool LSValue::operator == (const LSNumber*) const { return false; }
bool LSValue::operator == (const LSString*) const { return false; }
bool LSValue::operator == (const LSArray<LSValue*>*) const { return false; }
bool LSValue::operator == (const LSArray<int>*) const { return false; }
bool LSValue::operator == (const LSArray<double>*) const { return false; }
bool LSValue::operator == (const LSMap<LSValue*,LSValue*>*) const { return false; }
bool LSValue::operator == (const LSMap<LSValue*,int>*) const { return false; }
bool LSValue::operator == (const LSMap<LSValue*,double>*) const { return false; }
bool LSValue::operator == (const LSMap<int,LSValue*>*) const { return false; }
bool LSValue::operator == (const LSMap<int,int>*) const { return false; }
bool LSValue::operator == (const LSMap<int,double>*) const { return false; }
bool LSValue::operator == (const LSSet<LSValue*>*) const { return false; }
bool LSValue::operator == (const LSSet<int>*) const { return false; }
bool LSValue::operator == (const LSSet<double>*) const { return false; }
bool LSValue::operator == (const LSFunction*) const { return false; }
bool LSValue::operator == (const LSObject*) const { return false; }
bool LSValue::operator == (const LSClass*) const { return false; }

bool LSValue::operator < (const LSNull*) const {
	return typeID() < 1;
}
bool LSValue::operator < (const LSBoolean*) const {
	return typeID() < 2;
}
bool LSValue::operator < (const LSNumber*) const {
	return typeID() < 3;
}
bool LSValue::operator < (const LSString*) const {
	return typeID() < 4;
}
bool LSValue::operator < (const LSArray<LSValue*>*) const {
	return typeID() < 5;
}
bool LSValue::operator < (const LSArray<int>*) const {
	return typeID() < 5;
}
bool LSValue::operator < (const LSArray<double>*) const {
	return typeID() < 5;
}
bool LSValue::operator < (const LSMap<LSValue*,LSValue*>*) const {
	return typeID() < 6;
}
bool LSValue::operator < (const LSMap<LSValue*,int>*) const {
	return typeID() < 6;
}
bool LSValue::operator < (const LSMap<LSValue*,double>*) const {
	return typeID() < 6;
}
bool LSValue::operator < (const LSMap<int,LSValue*>*) const {
	return typeID() < 6;
}
bool LSValue::operator < (const LSMap<int,int>*) const {
	return typeID() < 6;
}
bool LSValue::operator < (const LSMap<int,double>*) const {
	return typeID() < 6;
}
bool LSValue::operator < (const LSSet<LSValue*>*) const {
	return typeID() < 7;
}
bool LSValue::operator < (const LSSet<int>*) const {
	return typeID() < 7;
}
bool LSValue::operator < (const LSSet<double>*) const {
	return typeID() < 7;
}
bool LSValue::operator < (const LSFunction*) const {
	return typeID() < 8;
}
bool LSValue::operator < (const LSObject*) const {
	return typeID() < 9;
}
bool LSValue::operator < (const LSClass*) const {
	return typeID() < 10;
}

bool LSValue::operator > (const LSNull* value) const {
	return not value->operator <(this) and not this->operator ==(value);
}
bool LSValue::operator > (const LSBoolean* value) const {
	return not value->operator <(this) and not this->operator ==(value);
}
bool LSValue::operator > (const LSNumber* value) const {
	return not value->operator <(this) and not this->operator ==(value);
}
bool LSValue::operator > (const LSString* value) const {
	return not value->operator <(this) and not this->operator ==(value);
}
bool LSValue::operator > (const LSArray<LSValue*>* value) const {
	return not value->operator <(this) and not this->operator ==(value);
}
bool LSValue::operator > (const LSArray<int>* value) const {
	return not value->operator <(this) and not this->operator ==(value);
}
bool LSValue::operator > (const LSArray<double>* value) const {
	return not value->operator <(this) and not this->operator ==(value);
}
bool LSValue::operator > (const LSMap<LSValue*,LSValue*>* value) const {
	return not value->operator <(this) and not this->operator ==(value);
}
bool LSValue::operator > (const LSMap<LSValue*,int>* value) const {
	return not value->operator <(this) and not this->operator ==(value);
}
bool LSValue::operator > (const LSMap<LSValue*,double>* value) const {
	return not value->operator <(this) and not this->operator ==(value);
}
bool LSValue::operator > (const LSMap<int,LSValue*>* value) const {
	return not value->operator <(this) and not this->operator ==(value);
}
bool LSValue::operator > (const LSMap<int,int>* value) const {
	return not value->operator <(this) and not this->operator ==(value);
}
bool LSValue::operator > (const LSMap<int,double>* value) const {
	return not value->operator <(this) and not this->operator ==(value);
}
bool LSValue::operator > (const LSSet<LSValue*>* value) const {
	return not value->operator <(this) and not this->operator ==(value);
}
bool LSValue::operator > (const LSSet<int>* value) const {
	return not value->operator <(this) and not this->operator ==(value);
}
bool LSValue::operator > (const LSSet<double>* value) const {
	return not value->operator <(this) and not this->operator ==(value);
}
bool LSValue::operator > (const LSFunction* value) const {
	return not value->operator <(this) and not this->operator ==(value);
}
bool LSValue::operator > (const LSObject* value) const {
	return not value->operator <(this) and not this->operator ==(value);
}
bool LSValue::operator > (const LSClass* value) const {
	return not value->operator <(this) and not this->operator ==(value);
}

bool LSValue::in(LSValue*) const { return false; }

LSValue* LSValue::at(const LSValue*) const {
	return LSNull::get();
}

LSValue** LSValue::atL(const LSValue*) {
	return nullptr;
}

LSValue* LSValue::attr(const LSValue* key) const {
	if (*((LSString*) key) == "class") {
		return getClass();
	}
	return LSNull::get();
}

LSValue** LSValue::attrL(const LSValue*) {
	return nullptr;
}

LSValue* LSValue::range(int, int) const {
	return clone();
}

LSValue* LSValue::rangeL(int, int) {
	return this;
}

LSValue* LSValue::abso() const {
	return LSNull::get();
}

//std::ostream& operator << (std::ostream& os, LSValue& value) {
//	value.print(os);
//	return os;
//}

bool LSValue::isInteger() const {
	if (const LSNumber* v = dynamic_cast<const LSNumber*>(this)) {
		return v->isInteger();
	}
	return false;
}

LSValue* get_value(int type, Json& json) {
	switch (type) {
		case 1: return LSNull::get();
		case 2: return new LSBoolean(json);
		case 3: return new LSNumber(json);
		case 4: return new LSString(json);
		case 5: return new LSArray<LSValue*>(json);
//		case 6: return new LSMap<LSValue*,LSValue*>(json); TODO
//		case 7: return new LSSet<LSValue*>(json);
		case 8: return new LSFunction(json);
		case 9: return new LSObject(json);
		case 10: return new LSClass(json);
	}
	return LSNull::get();
}

LSValue* LSValue::parse(Json& json) {

	int type = json["t"];
	Json data = json["v"];
	return get_value(type, data);
}

std::string LSValue::to_json() const {
	return "{\"t\":" + to_string(typeID()) + ",\"v\":" + json() + "}";
}

}

