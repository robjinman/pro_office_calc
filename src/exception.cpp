#include <sstream>
#include "exception.hpp"


using std::runtime_error;
using std::stringstream;


Exception::Exception(const utf8string_t& msg)
  : runtime_error(msg), m_msg(msg) {}

Exception::Exception(const utf8string_t& msg, const char* file, int line)
  : runtime_error(msg), m_msg(msg), m_file(file), m_line(line) {}

const char* Exception::what() const throw() {
  static utf8string_t msg;
  stringstream ss;

  ss << m_msg << " (FILE: " << m_file << ", LINE: " << m_line << ")";
  msg = ss.str();

  return msg.data();
}

void Exception::append(const utf8string_t& text) throw() {
  m_msg.append(text);
}

void Exception::prepend(const utf8string_t& text) throw() {
  m_msg.insert(0, text);
}

Exception::~Exception() throw() {}
