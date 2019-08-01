#ifndef OUTPUT_STREAM_HPP
#define OUTPUT_STREAM_HPP

#include <iostream>
#include <ostream>

namespace ls {

class OutputStream {
public:
	virtual std::ostream& stream() {
		return std::cout;
	}
	virtual void end() {
		std::cout << std::endl;
	}
};

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

}

#endif