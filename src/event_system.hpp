#ifndef __PROCALC_EVENT_SYSTEM_HPP__
#define __PROCALC_EVENT_SYSTEM_HPP__


#include <functional>
#include <map>
#include <list>
#include "event.hpp"


typedef std::function<void(const Event&)> handlerFunc_t;


class EventSystem {
  public:
    int listen(const std::string& name, handlerFunc_t fn);
    void forget(const std::string& name, int id);
    void fire(const Event& event);

  private:
    struct EventHandler {
      int id;
      handlerFunc_t handler;
    };

    std::map<std::string, std::list<EventHandler>> m_handlers;
};


#endif
