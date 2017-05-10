#ifndef __PROCALC_UTILS_HPP__
#define __PROCALC_UTILS_HPP__


#include <istream>
#include <string>
#ifdef DEBUG
#  include <iostream>
#  define DBG_PRINT(msg) std::cout << msg;
#else
#  define DBG_PRINT(msg)
#endif


std::string readString(std::istream& is);
void writeString(std::ostream& os, const std::string& s);


#endif
