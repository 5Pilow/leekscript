#ifndef TEST_HPP_
#define TEST_HPP_

#include <iostream>
#include <string>
#include "../src/vm/VM.hpp"
#include "../src/vm/value/LSNumber.hpp"
#include "../src/colors.h"
#include <thread>
#include "../src/environment/Environment.hpp"
#include "../src/analyzer/Result.hpp"

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
	std::map<std::thread::id, ls::Environment*> envs;
	std::map<std::thread::id, ls::Environment*> envs_legacy;
	int total;
	int success_count;
	int disabled;
	double parse_time;
	double compilation_time;
	double execution_time;
	int obj_created;
	int obj_deleted;
	int mpz_obj_created;
	int mpz_obj_deleted;
	static std::vector<std::string> failed_tests;
	static std::vector<std::string> disabled_tests;

public:

	class Input;

	Test();
	virtual ~Test();

	int all();
	void header(const std::string&);
	void section(const std::string&);
	void action(const std::string);

	Input code(const std::string& _code);
	Input DISABLED_code(const std::string& _code);
	Input code_v1(const std::string& _code);
	Input DISABLED_code_v1(const std::string& _code);
	Input file(const std::string& file_name);
	Input DISABLED_file(const std::string& _code);
	Input file_v1(const std::string& file_name);

	ls::Environment& getEnv(bool legacy = false);

	template <class T1, class T2>
	void test(const std::string& label, T1 value, T2 expected) {
		total++;
		std::ostringstream oss;
		if (value == expected) {
			success_count++;
			oss << C_GREEN << "OK   " << END_COLOR << ": " << label;
			oss <<  "  ===>  " << expected << std::endl;
			std::cout << oss.str();
		} else {
			oss << C_RED << "FAIL " << END_COLOR << ": " << label;
			oss << "  =/=>  " << expected << "  got  " << value << std::endl;
			std::cout << oss.str();
			failed_tests.push_back(oss.str());
		}
	}

	void test_general();
	void test_types();
	void test_legacy_array();
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
	void test_toplevel();
	void test_utils();

	class Input {
	public:
		Test* test;
		std::string label;
		std::string file_name;
		std::string code;
		bool file;
		bool v1;
		bool disabled;
		double parse_time = 0;
		double compilation_time = 0;
		double execution_time = 0;
		long int operation_limit = -1;
		ls::Result result;
		ls::Context* ctx = nullptr;

		Input(Test* test, const std::string& label, const std::string& file_name, const std::string& code, bool file = false, bool v1 = false, bool disabled = false) : test(test), label(label), file_name(file_name), code(code), file(file), v1(v1), disabled(disabled) {};
		void works();
		void equals(const std::string& expected);
		template <typename T>
		void almost(T expected, T delta = 1e-10);
		template <typename T>
		void between(T a, T b);
		void error(ls::Error::Type error, std::vector<std::string>&& params = {});
		void operations(int ops);
		void exception(ls::vm::Exception, std::vector<ls::vm::exception_frame> frames = {{"test", "main", 1}});
		void output(const std::string& expected);
		void quine();
		void type(const ls::Type*);
		Input& timeout(int ms);
		Input& ops_limit(long int ops);
		Input& context(ls::Context* ctx);

		ls::Result run(bool display_errors = true, bool ops = false);
		void pass(const std::string& expected);
		void fail(const std::string& expected, const std::string& actuel);
		void disable();
	};
};




#endif
