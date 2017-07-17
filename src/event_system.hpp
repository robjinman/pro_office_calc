#ifndef __PROCALC_EVENT_SYSTEM_HPP__
#define __PROCALC_EVENT_SYSTEM_HPP__


#include <functional>
#include <map>
#include "event.hpp"


typedef std::function<void(const Event&)> handlerFunc_t;


class EventSystem {
  public:
    int listen(const std::string& name, handlerFunc_t fn);
    void forget(int id);
    void fire(const Event& event);

  private:
    void fire(const std::string& name, const Event& event);

    std::map<std::string, std::map<int, handlerFunc_t>> m_handlers;
};


#endif
