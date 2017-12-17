#ifndef __PROCALC_EVENT_HPP__
#define __PROCALC_EVENT_HPP__


#include <string>


struct Event {
  Event(const char* name)
    : name(name) {}

  Event(const std::string& name)
    : name(name) {}

  std::string name;

  virtual ~Event() {}
};


#endif
