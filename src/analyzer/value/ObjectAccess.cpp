#include "ObjectAccess.hpp"
#include <chrono>
#include "../semantic/SemanticAnalyzer.hpp"
#include "VariableValue.hpp"
#include "../Program.hpp"
#include "../../standard/Module.hpp"
#include "../../analyzer/semantic/Callable.hpp"
#include "../semantic/Variable.hpp"
#include "../../type/Function_type.hpp"

namespace ls {

ObjectAccess::ObjectAccess(Environment& env, Token* token) : LeftValue(env), field(token) {
	attr_addr = nullptr;
	throws = true; // TODO doesn't always throw
}

ObjectAccess::~ObjectAccess() {}

bool ObjectAccess::isLeftValue() const {
	if (native_access_function.size()) {
		return false;
	}
	if (auto v = dynamic_cast<VariableValue*>(object.get())) {
		if (not v->isLeftValue()) return false;
	}
	return true;
}

void ObjectAccess::print(std::ostream& os, int indent, PrintOptions options) const {
	object->print(os, indent, options);
	os << "." << (field ? field->content : "");
	if (options.debug) {
		os << " " << type;
		os << " " << version;
	}
}

Location ObjectAccess::location() const {
	return {object->location().file, object->location().start, field ? field->location.end : dot->location.end};
}

void ObjectAccess::set_version(SemanticAnalyzer* analyzer, const std::vector<const Type*>& args, int level) {
	// std::cout << "ObjectAccess::set_version(" << args << ", " << level << ")" << std::endl;
	has_version = true;
	for (const auto& callable : call.callables) {
		for (const auto& m : callable->versions) {
			auto v = m.type->arguments();
			bool equals = v.size() == args.size() and std::equal(v.begin(), v.end(), args.begin(), [](const Type* a, const Type* b) {
				return a->operator == (b);
			});
			if (equals) {
				type = Type::fun(m.type->return_type(), v, (const Value*) this)->pointer();
				version = v;
				return;
			}
		}
	}
	version = args;
}

const Type* ObjectAccess::version_type(std::vector<const Type*> args) const {
	// std::cout << "ObjectAccess::version_tyoe(" << args << ")" << std::endl;
	for (const auto& callable : call.callables) {
		for (const auto& m : callable->versions) {
			auto version = m.type->arguments();
			bool equals = version.size() == args.size() and std::equal(version.begin(), version.end(), args.begin(), [](const Type* a, const Type* b) {
				return a->operator == (b);
			});
			if (equals) {
				return Type::fun(m.type->return_type(), args, (const Value*) this)->pointer();
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
	const auto& env = analyzer->env;
	// std::cout << "ObjectAccess::get_callable(" << argument_count << ")" << std::endl;

	auto vv = dynamic_cast<VariableValue*>(object.get());
	auto value_class = analyzer->globals["Value"]->clazz;

	std::string object_class_name = object->type->class_name();
	Class* object_class = nullptr;
	if (analyzer->globals.find(object_class_name) != analyzer->globals.end()) {
		object_class = analyzer->globals[object_class_name]->clazz;
	}

	if (not field) return {};

	// <object>.<method>
	if (object_class) {
		auto i = object_class->methods.find(field->content);
		if (i != object_class->methods.end()) {
			return { &i->second, nullptr, object.get() };
		}
	}
	// <object : Value>.<method>
	auto i = value_class->methods.find(field->content);
	if (i != value_class->methods.end() and i->second.is_compatible(argument_count + 1)) {
		return { &i->second, nullptr, object.get() };
	}
	// <class>.<field>
	if (object->type->is_class() and vv != nullptr) {
		auto std_class = analyzer->globals[vv->name]->clazz;
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
		auto type = Type::fun_object(env.any, { env.any, env.any });
		return { { { oss.str(), type, true, {}, {}, true } }, this, object.get() };
	}
	return {};
}

void ObjectAccess::pre_analyze(SemanticAnalyzer* analyzer) {
	object->pre_analyze(analyzer);
}

void ObjectAccess::analyze(SemanticAnalyzer* analyzer) {
	const auto& env = analyzer->env;

	// std::cout << "ObjectAccess analyse " << this << std::endl;

	object->analyze(analyzer);
	type = env.any;

	// Get the object class : 12 => Number
	object_class_name = object->type->class_name();
	Class* object_class = nullptr;
	if (object_class_name != "Value") {
		auto i = analyzer->globals.find(object_class_name);
		if (i != analyzer->globals.end()) {
			object_class = i->second->clazz;
		}
	}

	if (not field) return;

	// Static attribute? (Number.PI <= static attr)
	auto vv = dynamic_cast<VariableValue*>(object.get());

	bool found = false;
	if (object->type->is_class() and vv != nullptr and analyzer->globals.find(vv->name) != analyzer->globals.end()) {

		auto std_class = analyzer->globals[vv->name]->clazz;
		auto i = std_class->methods.find(field->content);
		if (i != std_class->methods.end()) {

			auto& method = i->second;
			int i = 0;
			for (const auto& m : method.versions) {
				versions.insert({ m.type->arguments(), std_class->name + "." + field->content + "." + std::to_string(i) });
				i++;
			}
			type = Type::fun(method.versions[0].type->return_type(), method.versions[0].type->arguments(), (ObjectAccess*) this)->pointer();
			default_version_fun = std_class->name + "." + field->content;
			class_method = true;
			call = { &method };
			found = true;
		}
	}

	if (!found and object->type->is_class() and vv != nullptr and analyzer->globals.find(vv->name) != analyzer->globals.end()) {

		auto std_class = analyzer->globals[vv->name]->clazz;

		auto i = std_class->static_fields.find(field->content);
		if (i != std_class->static_fields.end()) {

			const auto& mod_field = i->second;
			type = mod_field.type;
			#if COMPILER
			if (mod_field.static_fun != nullptr) {
				static_access_function = mod_field.static_fun;
			}
			#endif
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

	// Attribute? Fields and methods ([1, 2, 3].length, 12.abs)
	if (!found and object_class != nullptr) {
		// Attribute : (x -> x).return

		auto current_class = object_class;
		while (current_class) {
			auto i = current_class->fields.find(field->content);
			if (i != current_class->fields.end()) {
				const auto& f = i->second;
				type = f.type;
				#if COMPILER
				if (f.fun != nullptr) {
					access_function = f.fun;
				}
				#endif
				if (f.native_fun != nullptr) {
					native_access_function = current_class->name + "." + f.name;
				}
				found = true;
				break;
			}

			// Method : 12.abs
			auto j = current_class->methods.find(field->content);
			if (j != current_class->methods.end()) {
				for (const auto& m : j->second.versions) {
					if (!m.addr) continue;
					versions.insert({m.type->arguments(), current_class->name + "." + field->content});
				}
				type = j->second.versions[0].type->pointer();
				default_version_fun = current_class->name + "." + field->content;
				class_method = true;
				found = true;
				break;
			}

			current_class = current_class->parent;
		}

		if (not found) {
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

Completion ObjectAccess::autocomplete(SemanticAnalyzer& analyzer, size_t position) const {
	// std::cout << "ObjectAccess complete " << position << std::endl;
	if (position >= dot->location.end.raw) {

		Completion completion {  object->type };
		Location field_location = field ? field->location : dot->location.after();
		auto letters = field ? field->content : "";
		// std::cout << "Object type = " << object->type->class_name() << std::endl;

		// Number.<xxx>
		auto vv = dynamic_cast<VariableValue*>(object.get());
		if (object->type->is_class() and vv) {
			auto std_class = analyzer.globals[vv->name]->clazz;
			for (const auto& method : std_class->methods) {
				if (method.first.find(letters) == std::string::npos) continue;
				for (const auto& version : method.second.versions) {
					if (version.flags & Module::PRIVATE) continue;
					completion.items.push_back({ method.first, CompletionType::METHOD, version.type, field_location });
				}
			}
			for (const auto& field : std_class->static_fields) {
				if (field.first.find(letters) == std::string::npos) continue;
				if (field.second.flags & Module::PRIVATE) continue;
				completion.items.push_back({ field.first, CompletionType::FIELD, field.second.type, field_location });
			}
		}

		// 123.<xxx>
		Class* current_class = analyzer.globals[object->type->class_name()]->clazz;
		while (current_class) {
			for (const auto& field : current_class->fields) {
				if (field.first.find(letters) == std::string::npos) continue;
				completion.items.push_back({ field.first, CompletionType::FIELD, field.second.type, field_location });
			}
			for (const auto& method : current_class->methods) {
				if (method.second.versions.front().flags & Module::PRIVATE) continue;
				if (method.first.find(letters) == std::string::npos) continue;
				for (const auto& version : method.second.versions) {
					completion.items.push_back({ method.first, CompletionType::METHOD, version.type, field_location });
				}
			}
			current_class = current_class->parent;
		}
		return completion;
	}
	return {};
}

Hover ObjectAccess::hover(SemanticAnalyzer& analyzer, size_t position) const {
	if (position < dot->location.end.raw) {
		return object->hover(analyzer, position);
	} else {
		return { type, location() };
	}
}

#if COMPILER
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
	auto r = c.insn_invoke(type, {c.get_vm(), o, k}, "Value.attr");
	object->compile_end(c);
	return r;
}

Compiler::value ObjectAccess::compile_version(Compiler& c, std::vector<const Type*> version) const {
	if (class_method) {
		// Method symbol like "Number.abs"
		return c.get_symbol(versions.at(version), Type::fun(c.env.void_, {})->pointer());
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
#endif

std::unique_ptr<Value> ObjectAccess::clone(Block* parent) const {
	auto oa = std::make_unique<ObjectAccess>(type->env, field);
	oa->object = object->clone(parent);
	return oa;
}

}
