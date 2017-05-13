#ifndef __PROCALC_UPDATE_LOOP_HPP__
#define __PROCALC_UPDATE_LOOP_HPP__


#include <memory>
#include <list>
#include <functional>


class QTimer;

class UpdateLoop {
  public:
    UpdateLoop(std::unique_ptr<QTimer> timer, int interval);

    void add(std::function<bool()> fn, std::function<void()> fnOnFinish = []() {});
    int size() const;
    void update();
    double fps() const;

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
