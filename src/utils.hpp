#ifndef __PROCALC_UTILS_HPP__
#define __PROCALC_UTILS_HPP__


#include <istream>
#include <string>
#include <map>
#ifdef DEBUG
#  include <iostream>
#  define DBG_PRINT(msg) std::cout << msg;
#else
#  define DBG_PRINT(msg)
#endif
#include "platform.hpp"


#define PI 3.141592653589793
#define DEG_TO_RAD(x) (x * PI / 180.0)
#define RAD_TO_DEG(x) (x * 180.0 / PI)


std::string readString(std::istream& is);
void writeString(std::ostream& os, const std::string& s);

template<class T>
bool ltelte(T a, T b, T c) {
  return a <= b && b <= c;
}

template<class T>
T smallest(T a, T b) {
  return a < b ? a : b;
}

template<class T, class U>
bool contains(const std::map<T, U>& map, const T& key) {
  return map.find(key) != map.end();
}

typedef std::hash<std::string> hashString;

#ifdef DEBUG
class QRect;
class QRectF;
std::ostream& operator<<(std::ostream& os, const QRect& rect);
std::ostream& operator<<(std::ostream& os, const QRectF& rect);
#endif


#endif
