#ifndef __PROCALC_REQUEST_STATE_CHANGE_EVENT_HPP__
#define __PROCALC_REQUEST_STATE_CHANGE_EVENT_HPP__


#include "event.hpp"


struct RequestStateChangeEvent : public Event {
  RequestStateChangeEvent(int stateId, bool hardReset = false)
    : Event("requestStateChange"),
      stateId(stateId),
      hardReset(hardReset) {}

    int stateId;
    bool hardReset;
};


#endif
