#ifndef __PROCALC_EVENT_HPP__
#define __PROCALC_EVENT_HPP__


#include <memory>
#include <string>


class Event {
  public:
    Event(const char* name)
      : name(name) {}

    Event(const std::string& name)
      : name(name) {}

    std::string name;

    virtual ~Event() {}
};

typedef std::unique_ptr<Event> pEvent_t;


#endif
