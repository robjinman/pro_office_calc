#ifndef __PROCALC_UTILS_HPP__
#define __PROCALC_UTILS_HPP__


#include <istream>
#include <string>
#include <memory>
#include <map>
#include <vector>
#include <QPointer>
#ifdef DEBUG
#  include <iostream>
#  define DBG_PRINT(msg) std::cout << msg;
#  define DBG_PRINT_VAR(var) std::cout << #var" = " << var << "\n";
#else
#  define DBG_PRINT(msg)
#endif
#include "platform.hpp"

#define PI 3.141592653589793
#define DEG_TO_RAD(x) (x * PI / 180.0)
#define RAD_TO_DEG(x) (x * 180.0 / PI)


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

template <class T>
using CustomQtObjDeleter = std::function<void(const T*)>;

template <class T>
using QtObjPtr = std::unique_ptr<T, CustomQtObjDeleter<T>>;

template <class T, typename... Args>
auto makeQtObjPtr(Args&&... args) {
  // Instantiate the object in an exception-safe way
  auto tmp = std::make_unique<T>(std::forward<Args>(args)...);

  // A QPointer will keep track of whether the object is still alive
  QPointer<T> qPtr(tmp.get());

  CustomQtObjDeleter<T> deleter = [qPtr](const T*) {
    if (qPtr) {
      qPtr->deleteLater();
    }
  };

  return QtObjPtr<T>(tmp.release(), std::move(deleter));
}

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
