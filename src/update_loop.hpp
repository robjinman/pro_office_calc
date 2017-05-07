#ifndef __PROCALC_UPDATE_LOOP_HPP__
#define __PROCALC_UPDATE_LOOP_HPP__


#include <memory>
#include <list>


struct IUpdatable {
  virtual bool update() = 0;
  virtual ~IUpdatable() {}
};


class QTimer;

class UpdateLoop {
  public:
    UpdateLoop(std::unique_ptr<QTimer> timer, int interval);

    void addObject(std::weak_ptr<IUpdatable> obj);
    int numObjects() const;
    void update();

  private:
    std::unique_ptr<QTimer> m_timer;
    int m_interval;
    std::list<std::weak_ptr<IUpdatable>> m_objects;
};


#endif
