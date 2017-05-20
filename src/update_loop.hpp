#ifndef __PROCALC_UPDATE_LOOP_HPP__
#define __PROCALC_UPDATE_LOOP_HPP__


#include <memory>
#include <list>
#include <functional>
#include <QObject>
#include <QTimer>


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

    std::unique_ptr<QTimer> m_timer;
    int m_interval;
    std::list<FuncPair> m_functions;
};


#endif
