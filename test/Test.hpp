#ifndef TEST_HPP_
#define TEST_HPP_

#include <iostream>
#include <string>
#include "../src/vm/VM.hpp"
#include "../src/vm/value/LSNumber.hpp"

class OutputStringStream : public ls::OutputStream {
	std::ostringstream oss;
public:
	virtual std::ostream& stream() override {
		return oss;
	}
	virtual void end() override {
		oss << std::endl;
	}
	std::string str() const {
		return oss.str();
	}
};

class Test {
private:
	ls::VM vm;
	ls::VM vmv1;
	int total;
	int success_count;
	int disabled;
	double compilation_time;
	double execution_time;
	int obj_created;
	int obj_deleted;
	int mpz_obj_created;
	int mpz_obj_deleted;
	static std::vector<std::string> failed_tests;

public:

	class Input;

	Test();
	virtual ~Test();

	int all();
	void header(std::string);
	void section(std::string);

	Input code(const std::string& _code);
	Input DISABLED_code(const std::string& _code);
	Input code_v1(const std::string& _code);
	Input DISABLED_code_v1(const std::string& _code);
	Input file(const std::string& file_name);
	Input DISABLED_file(const std::string& _code);
	Input file_v1(const std::string& file_name);

	void test_general();
	void test_types();
	void test_operations();
	void test_operators();
	void test_references();
	void test_system();
	void test_objects();
	void test_strings();
	void test_numbers();
	void test_booleans();
	void test_arrays();
	void test_map();
	void test_set();
	void test_functions();
	void test_loops();
	void test_classes();
	void test_files();
	void test_doc();
	void test_intervals();
	void test_json();
	void test_exceptions();
	void test_utils();

	class Input {
	public:
		Test* test;
		std::string name;
		std::string code;
		bool file;
		bool v1;
		bool disabled;
		float compilation_time = 0;
		float execution_time = 0;
		long int operation_limit = 0;
		ls::VM::Result result;
		ls::Context* ctx = nullptr;

		Input(Test* test, const std::string& name, const std::string& code,
			bool file = false, bool v1 = false, bool disabled = false) : test(test), name(name), code(code), file(file), v1(v1), disabled(disabled) {};
		void works();
		void equals(std::string expected);
		template <typename T>
		void almost(T expected, T delta = 1e-10);
		template <typename T>
		void between(T a, T b);
		void semantic_error(ls::SemanticError::Type error, std::vector<std::string> params);
		void syntaxic_error(ls::SyntaxicalError::Type error, std::vector<std::string> params);
		void lexical_error(ls::LexicalError::Type error);
		void operations(int ops);
		void exception(ls::vm::Exception, std::vector<ls::vm::exception_frame> frames = {{"main", 1}});
		void output(std::string expected);
		void quine();
		void type(ls::Type);
		Input& timeout(int ms);
		Input& ops_limit(long int ops);
		Input& context(ls::Context* ctx);

		ls::VM::Result run(bool display_errors = true, bool ops = false);
		void pass(std::string expected);
		void fail(std::string expected, std::string actuel);
		void disable();
	};
};




#endif
