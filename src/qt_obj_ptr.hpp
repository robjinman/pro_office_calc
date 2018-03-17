#ifndef __PROCALC_QT_OBJ_PTR_HPP__
#define __PROCALC_QT_OBJ_PTR_HPP__


#include <type_traits>
#include <functional>
#include <memory>
#include <QPointer>


template <class T>
using CustomQtObjDeleter = std::function<void(const QObject*)>;

template <class T>
using QtObjPtr = std::unique_ptr<T, CustomQtObjDeleter<T>>;

template <class T, typename... Args>
auto makeQtObjPtr(Args&&... args) {
  static_assert(std::is_base_of<QObject, T>::value, "Template arg must derive from QObject");

  // Instantiate the object in an exception-safe way
  auto tmp = std::make_unique<T>(std::forward<Args>(args)...);

  // A QPointer will keep track of whether the object is still alive
  QPointer<T> qPtr(tmp.get());

  CustomQtObjDeleter<T> deleter = [qPtr](const QObject*) {
    if (qPtr) {
      qPtr->deleteLater();
    }
  };

  return QtObjPtr<T>(tmp.release(), std::move(deleter));
}

template <class T>
auto makeQtObjPtrFromRawPtr(T* rawPtr) {
  static_assert(std::is_base_of<QObject, T>::value, "Template arg must derive from QObject");

  QPointer<T> qPtr(rawPtr);

  CustomQtObjDeleter<T> deleter = [qPtr](const QObject*) {
    if (qPtr) {
      qPtr->deleteLater();
    }
  };

  return QtObjPtr<T>(rawPtr, std::move(deleter));
}


#endif
