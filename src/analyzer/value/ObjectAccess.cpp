#include "ObjectAccess.hpp"
#include <chrono>
#include "../semantic/SemanticAnalyzer.hpp"
#include "VariableValue.hpp"
#include "../../vm/value/LSNull.hpp"
#include "../../vm/value/LSString.hpp"
#include "../../vm/value/LSFunction.hpp"
#include "../../vm/value/LSClass.hpp"
#include "../../vm/Program.hpp"
#include "../../vm/Module.hpp"
#include "../../compiler/semantic/Callable.hpp"
#include "../semantic/Variable.hpp"
#include "../../type/Function_type.hpp"

namespace ls {

ObjectAccess::ObjectAccess(Token* token) : field(token) {
	attr_addr = nullptr;
	throws = true; // TODO doesn't always throw
}

ObjectAccess::~ObjectAccess() {
	if (ls_function != nullptr) {
		delete ls_function;
	}
}

bool ObjectAccess::isLeftValue() const {
	if (native_access_function.size()) {
		return false;
	}
	if (auto v = dynamic_cast<VariableValue*>(object.get())) {
		if (not v->isLeftValue()) return false;
	}
	return true;
}

void ObjectAccess::print(std::ostream& os, int indent, bool debug, bool condensed) const {
	object->print(os, indent, debug, condensed);
	os << "." << field->content;
	if (debug) {
		os << " " << type;
		os << " " << version;
	}
}

Location ObjectAccess::location() const {
	return {field->location.file, object->location().start, field->location.end};
}

void ObjectAccess::set_version(SemanticAnalyzer* analyzer, const std::vector<const Type*>& args, int level) {
	// std::cout << "ObjectAccess::set_version(" << args << ", " << level << ")" << std::endl;
	has_version = true;
	if (call) {
		for (const auto& m : call->callable->versions) {
			auto v = m->type->arguments();
			bool equals = v.size() == args.size() and std::equal(v.begin(), v.end(), args.begin(), [](const Type* a, const Type* b) {
				return a->operator == (b);
			});
			if (equals) {
				type = Type::fun(m->type->return_type(), v, (const Value*) this)->pointer();
				version = v;
				return;
			}
		}
	}
	version = args;
}

const Type* ObjectAccess::version_type(std::vector<const Type*> args) const {
	// std::cout << "ObjectAccess::version_tyoe(" << args << ")" << std::endl;
	if (call) {
		for (const auto& m : call->callable->versions) {
			auto version = m->type->arguments();
			bool equals = version.size() == args.size() and std::equal(version.begin(), version.end(), args.begin(), [](const Type* a, const Type* b) {
				return a->operator == (b);
			});
			if (equals) {
				return Type::fun(m->type->return_type(), args, (const Value*) this)->pointer();
			}
		}
	}
	return type;
}

const Type* ObjectAccess::will_take(SemanticAnalyzer* analyzer, const std::vector<const Type*>& args, int level) {
	// std::cout << "OA will take " << args << std::endl;
	set_version(analyzer, args, 1);
	return type;
}

Call ObjectAccess::get_callable(SemanticAnalyzer* analyzer, int argument_count) const {
	// std::cout << "ObjectAccess::get_callable(" << argument_count << ")" << std::endl;

	auto vv = dynamic_cast<VariableValue*>(object.get());
	auto value_class = (LSClass*) analyzer->vm->internal_vars["Value"]->lsvalue;

	std::string object_class_name = object->type->class_name();
	LSClass* object_class = nullptr;
	if (analyzer->vm->internal_vars.find(object_class_name) != analyzer->vm->internal_vars.end()) {
		object_class = (LSClass*) analyzer->vm->internal_vars[object_class_name]->lsvalue;
	}

	// <object>.<method>
	if (object_class) {
		auto i = object_class->methods.find(field->content);
		if (i != object_class->methods.end()) {
			return { &i->second, object.get() };
		}
	}
	// <object : Value>.<method>
	auto i = value_class->methods.find(field->content);
	if (i != value_class->methods.end() and i->second.is_compatible(argument_count + 1)) {
		return { &i->second, object.get() };
	}
	// <class>.<field>
	if (object->type->is_class() and vv != nullptr) {
		auto std_class = (LSClass*) analyzer->vm->internal_vars[vv->name]->lsvalue;
		// <class>.<method>
		auto i = std_class->methods.find(field->content);
		if (i != std_class->methods.end()) {
			return { &i->second };
		}
		// Value.<method>
		i = value_class->methods.find(field->content);
		if (i != value_class->methods.end() and i->second.is_compatible(argument_count)) {
			return { &i->second };
		}
	}
	if (not object->type->is_class()) {
		std::ostringstream oss;
		oss << object.get() << "." << field->content;
		auto type = Type::fun_object(Type::any, {Type::any, Type::any});
		return { new Callable { new CallableVersion { oss.str(), type, this, {}, {}, true, true } }, object.get() };
	}
	return { (Callable*) nullptr };
}

void ObjectAccess::pre_analyze(SemanticAnalyzer* analyzer) {
	object->pre_analyze(analyzer);
}

void ObjectAccess::analyze(SemanticAnalyzer* analyzer) {

	// std::cout << "ObjectAccess analyse " << this << std::endl;

	object->analyze(analyzer);
	type = Type::any;

	// Get the object class : 12 => Number
	object_class_name = object->type->class_name();
	LSClass* object_class = nullptr;
	if (object_class_name != "Value") {
		auto i = analyzer->vm->internal_vars.find(object_class_name);
		if (i != analyzer->vm->internal_vars.end()) {
			object_class = (LSClass*) i->second->lsvalue;
		}
	}

	// Static attribute? (Number.PI <= static attr)
	auto vv = dynamic_cast<VariableValue*>(object.get());

	bool found = false;
	if (object->type->is_class() and vv != nullptr) {

		auto std_class = (LSClass*) analyzer->vm->internal_vars[vv->name]->lsvalue;
		auto i = std_class->methods.find(field->content);
		if (i != std_class->methods.end()) {

			auto& method = i->second;
			int i = 0;
			for (const auto& m : method.versions) {
				versions.insert({m->type->arguments(), std_class->name + "." + field->content + "." + std::to_string(i)});
				i++;
			}
			type = Type::fun(method.versions[0]->type->return_type(), method.versions[0]->type->arguments(), (ObjectAccess*) this)->pointer();
			default_version_fun = std_class->name + "." + field->content;
			class_method = true;
			call = new Call { &method };
			found = true;
		}
	}
	if (!found and object->type->is_class() and vv != nullptr) {

		auto std_class = (LSClass*) analyzer->vm->internal_vars[vv->name]->lsvalue;

		auto i = std_class->static_fields.find(field->content);
		if (i != std_class->static_fields.end()) {

			const auto& mod_field = i->second;
			type = mod_field.type;

			if (mod_field.static_fun != nullptr) {
				static_access_function = mod_field.static_fun;
			}
			if (mod_field.addr != nullptr) {
				attr_addr = mod_field.addr;
			}
			if (mod_field.native_fun != nullptr) {
				native_static_access_function = std_class->name + std::string(".") + mod_field.name;
			}
			field_type = mod_field.type;
			found = true;
		}
	}

	auto value_class = (LSClass*) analyzer->vm->internal_vars["Value"]->lsvalue;

	// Attribute? Fields and methods ([1, 2, 3].length, 12.abs)
	if (!found and object_class != nullptr) {
		// Attribute : (x -> x).return
		auto i = object_class->fields.find(field->content);
		if (i != object_class->fields.end()) {
			const auto& f = i->second;
			type = f.type;
			if (f.fun != nullptr) {
				access_function = f.fun;
			}
			if (f.native_fun != nullptr) {
				native_access_function = object_class->name + "." + f.name;
			}
		} else {
			// Attribute in Value?
			auto i = value_class->fields.find(field->content);
			if (i != value_class->fields.end()) {
				auto f = i->second;
				type = f.type;
				class_field = true;
				if (f.fun != nullptr) {
					access_function = f.fun;
				}
				if (f.native_fun != nullptr) {
					native_access_function = "Value." + f.name;
				}
			} else {
				// Method : 12.abs
				auto i = object_class->methods.find(field->content);
				if (i != object_class->methods.end()) {
					for (const auto& m : i->second.versions) {
						if (!m->addr) continue;
						versions.insert({m->type->arguments(), object_class->name + "." + field->content});
					}
					type = i->second.versions[0]->type->pointer();
					default_version_fun = object_class->name + "." + field->content;
					class_method = true;
				} else {
					auto i = value_class->methods.find(field->content);
					if (i != value_class->methods.end()) {
						for (const auto& m : i->second.versions) {
							if (!m->addr) continue;
							versions.insert({m->type->arguments(), "Value." + field->content});
						}
						type = i->second.versions[0]->type;
						default_version_fun = "Value." + field->content;
						class_field = true;
					} else {
						if (object_class->name != "Object") {
							if (object->type->is_class() and vv != nullptr) {
								analyzer->add_error({Error::Type::NO_SUCH_ATTRIBUTE, location(), field->location, {field->content, vv->name}});
							} else {
								analyzer->add_error({Error::Type::NO_SUCH_ATTRIBUTE, location(), field->location, {field->content, object_class->name}});
							}
							return;
						}
					}
				}
			}
		}
	}
}

Compiler::value ObjectAccess::compile(Compiler& c) const {

	// Special case for custom attributes, accessible via a function
	// Static attributes : Number.PI
	if (static_access_function != nullptr) {
		return static_access_function(c);
	}
	if (native_static_access_function.size()) {
		return c.insn_call(field_type, {}, native_static_access_function);
	}

	// Field with an access function : 12.class
	if (access_function != nullptr) {
		auto obj = object->compile(c);
		object->compile_end(c);
		return access_function(c, obj);
	}
	if (native_access_function.size()) {
		auto obj = object->compile(c);
		object->compile_end(c);
		return c.insn_call(type, {obj}, native_access_function);
	}
	if (attr_addr) {
		return c.insn_load(c.get_symbol(object->to_string() + "." + field->content, type->pointer()));
	}

	// Class method : 12.abs
	if (class_method || class_field) {
		const auto& fun = has_version and versions.find(version) != versions.end() ? versions.at(version) : default_version_fun;
		return c.get_symbol(fun, Type::fun(type->return_type(), type->arguments())->pointer());
	}

	// Default : object.attr
	auto o = object->compile(c);
	auto k = c.new_const_string(field->content);
	auto r = c.insn_invoke(type, {o, k}, "Value.attr");
	object->compile_end(c);
	return r;
}

Compiler::value ObjectAccess::compile_version(Compiler& c, std::vector<const Type*> version) const {
	if (class_method) {
		// Method symbol like "Number.abs"
		return c.get_symbol(versions.at(version), Type::fun()->pointer());
	}
	assert(false && "ObjectAccess::compile_version must be on a class method.");
}

Compiler::value ObjectAccess::compile_l(Compiler& c) const {
	auto o = [&]() { if (object->isLeftValue()) {
		return c.insn_load(((LeftValue*) object.get())->compile_l(c));
	} else {
		return object->compile(c);
	}}();
	object->compile_end(c);
	auto k = c.new_const_string(field->content);
	return c.insn_invoke(type->pointer(), {o, k}, "Value.attrL");
}

std::unique_ptr<Value> ObjectAccess::clone() const {
	auto oa = std::make_unique<ObjectAccess>(field);
	oa->object = object->clone();
	return oa;
}

}
