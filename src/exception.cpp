#include <sstream>
#include "exception.hpp"


using std::runtime_error;
using std::string;
using std::stringstream;


//===========================================
// Exception::Exception
//===========================================
Exception::Exception(const string& msg)
  : runtime_error(msg), m_msg(msg) {}

//===========================================
// Exception::Exception
//===========================================
Exception::Exception(const string& msg, const char* file, int line)
  : runtime_error(msg), m_msg(msg), m_file(file), m_line(line) {}

//===========================================
// Exception::what
//===========================================
const char* Exception::what() const throw() {
  static string msg;
  stringstream ss;

  ss << m_msg << " (FILE: " << m_file << ", LINE: " << m_line << ")";
  msg = ss.str();

  return msg.data();
}

//===========================================
// Exception::append
//===========================================
void Exception::append(const string& text) throw() {
  m_msg.append(text);
}

//===========================================
// Exception::prepend
//===========================================
void Exception::prepend(const string& text) throw() {
  m_msg.insert(0, text);
}

//===========================================
// Exception::~Exception
//===========================================
Exception::~Exception() throw() {}
