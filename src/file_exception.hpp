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
    FileException(const std::string& msg)
      : Exception(msg) {}

    FileException(const std::string& msg, const char* file, int line)
      : Exception(msg, file, line) {}

    FileException(const std::string& msg, const std::string& path, const char* file, int line)
      : Exception(msg, file, line), m_file(path) {

      append(" (path: ");
      append(path);
      append(")");
    }

    std::string getFilePath() const throw() {
      return m_file;
    }

    virtual ~FileException() throw() {}

  private:
    std::string m_file;
};


#endif
