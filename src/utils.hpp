#ifndef __PROCALC_UTILS_HPP__
#define __PROCALC_UTILS_HPP__


#include <istream>
#include <sstream>
#include <string>
#include <memory>
#include <map>
#include <vector>
#include <QPointer>
#include "exception.hpp"


//===========================================
// Debug Build
//
#ifdef DEBUG
#include <iostream>

#define DBG_PRINT(msg) std::cout << msg;
#define DBG_PRINT_VAR(var) std::cout << #var" = " << var << "\n";

// TODO
#define DYNAMIC_CAST static_cast

//===========================================

//===========================================
// Release Build
//
#else

#define DBG_PRINT(msg)
#define DBG_PRINT_VAR(var)

#define DYNAMIC_CAST static_cast

#endif
//===========================================


#include "platform.hpp"

#define PI 3.141592653589793
#define DEG_TO_RAD(x) (x * PI / 180.0)
#define RAD_TO_DEG(x) (x * 180.0 / PI)

template<class T>
inline typename T::mapped_type& getValueFromMap(T& map, const typename T::key_type& key,
  const char* file, int line) {

  auto it = map.find(key);
  if (it == map.end()) {
    std::stringstream ss;
    ss << "Map does not contain key '" << key << "'";
    throw Exception(ss.str(), file, line);
  }

  return it->second;
}

template<class T>
inline const typename T::mapped_type& getValueFromMap(const T& map, const typename T::key_type& key,
  const char* file, int line) {

  return getValueFromMap<T>(const_cast<T&>(map), key, file, line);
}

#define GET_VALUE(mapInst, keyName) \
  getValueFromMap(mapInst, keyName, __FILE__, __LINE__)

std::string readString(std::istream& is);
void writeString(std::ostream& os, const std::string& s);

std::vector<std::string> splitString(const std::string& s, char delim);

template<class T>
bool ltelte(T a, T b, T c) {
  return a <= b && b <= c;
}

template<class T>
T smallest(T a, T b) {
  return a < b ? a : b;
}

template<class T>
T largest(T a, T b) {
  return a > b ? a : b;
}

template<class T, class U>
bool contains(const std::map<T, U>& map, const T& key) {
  return map.find(key) != map.end();
}

template<class T>
void erase(T& container, const typename T::iterator& it) {
  if (it != container.end()) {
    container.erase(it);
  }
}

long randomSeed();

typedef std::hash<std::string> hashString;

#ifdef DEBUG
class QRect;
class QRectF;
class QPoint;
class QPointF;
std::ostream& operator<<(std::ostream& os, const QRect& rect);
std::ostream& operator<<(std::ostream& os, const QRectF& rect);
std::ostream& operator<<(std::ostream& os, const QPoint& p);
std::ostream& operator<<(std::ostream& os, const QPointF& p);
#endif


#endif
