#ifndef __PROCALC_EXCEPTION_HPP__
#define __PROCALC_EXCEPTION_HPP__


#include <stdexcept>
#include "strings.hpp"


#define EXCEPTION(x) { \
  std::stringstream ss; \
  ss << x; \
  throw Exception(x, __FILE__, __LINE__); \
}


class Exception : public std::runtime_error {
  public:
    Exception(const utf8string_t& msg);
    Exception(const utf8string_t& msg, const char* file, int line);

    virtual const char* what() const throw();

    void append(const utf8string_t& text) throw();
    void prepend(const utf8string_t& text) throw();

    virtual ~Exception() throw();

  private:
    utf8string_t m_msg;
    utf8string_t m_file;
    int m_line;
};


#endif
