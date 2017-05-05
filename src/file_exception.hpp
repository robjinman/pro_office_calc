#ifndef __PROCALC_FILE_EXCEPTION_HPP__
#define __PROCALC_FILE_EXCEPTION_HPP__


#include "exception.hpp"


#define FILE_EXCEPTION(x, file) { \
  stringstream ss; \
  ss << x; \
  throw FileException(ss.str(), file, __FILE__, __LINE__); \
}


class FileException : public Exception {
  public:
    FileException(const utf8string_t& msg)
      : Exception(msg) {}

    FileException(const utf8string_t& msg, const char* file, int line)
      : Exception(msg, file, line) {}

    FileException(const utf8string_t& msg, const utf8string_t& path, const char* file, int line)
      : Exception(msg, file, line), m_file(path) {

      append(" (path: ");
      append(path);
      append(")");
    }

    utf8string_t getFilePath() const throw() {
      return m_file;
    }

    virtual ~FileException() throw() {}

  private:
    utf8string_t m_file;
};


#endif
