#ifndef __PROCALC_UPDATE_LOOP_HPP__
#define __PROCALC_UPDATE_LOOP_HPP__


#include <memory>
#include <list>
#include <functional>


class QTimer;

class UpdateLoop {
  public:
    UpdateLoop(std::unique_ptr<QTimer> timer, int interval);

    void add(std::function<bool()> fn);
    int size() const;
    void update();

  private:
    std::unique_ptr<QTimer> m_timer;
    int m_interval;
    std::list<std::function<bool()>> m_functions;
};


#endif
