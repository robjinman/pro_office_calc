#ifndef __PROCALC_EXCEPTION_HPP__
#define __PROCALC_EXCEPTION_HPP__


#include <stdexcept>
#include <sstream>


#define EXCEPTION(x) { \
  std::stringstream ss; \
  ss << x; \
  throw Exception(ss.str(), __FILE__, __LINE__); \
}

#define ASSERT(cond) \
  if (!(cond)) { \
    EXCEPTION("Assertion failed"); \
  }

#define ASSERT_EQ(actual, expected) \
  if ((actual) != (expected)) { \
    EXCEPTION("Assertion failed (" << actual << " != " << expected << ")"); \
  }


class Exception : public std::runtime_error {
  public:
    Exception(const std::string& msg);
    Exception(const std::string& msg, const char* file, int line);

    virtual const char* what() const throw();

    void append(const std::string& text) throw();
    void prepend(const std::string& text) throw();

    virtual ~Exception() throw();

  private:
    std::string m_msg;
    std::string m_file;
    int m_line;
};


#endif
