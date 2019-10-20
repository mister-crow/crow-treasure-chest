#pragma once

// required by std::string
#include <string>

// required by std::stringstream
#include <sstream>


namespace crowbox {


class Exception : public std::exception {
public:
	Exception() = default;

	explicit Exception(const std::string & msg)
		: std::exception(), m_error_msg(msg) {
	}

	virtual const char * what() const noexcept {
		return this->m_error_msg.c_str();
	}

private:
	std::string m_error_msg;
};


#define EXCEPTION(msg) create_detailed_exception(msg, __FILE__, __LINE__)

Exception create_detailed_exception(
		const char * msg, const char * file, unsigned line) {
	std::stringstream ss;
	ss << "ERROR: " << msg << " in file " << file << " line " << line;
	return Exception(ss.str());
}

}
