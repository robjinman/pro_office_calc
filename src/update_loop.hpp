#ifndef __PROCALC_UPDATE_LOOP_HPP__
#define __PROCALC_UPDATE_LOOP_HPP__


#include <list>
#include <functional>
#include <QObject>
#include <QTimer>
#include "qt_obj_ptr.hpp"


class UpdateLoop : QObject {
  Q_OBJECT

  public:
    UpdateLoop(int interval);

    void add(std::function<bool()> fn, std::function<void()> fnOnFinish = []() {});
    int size() const;
    double fps() const;
    void finishAll();

  private slots:
    void tick();

  private:
    struct FuncPair {
      std::function<bool()> fnPeriodic;
      std::function<void()> fnFinish;
    };

    QtObjPtr<QTimer> m_timer;
    int m_interval;
    std::list<FuncPair> m_functions;
};


#endif
