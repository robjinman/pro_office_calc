#ifndef __PROCALC_NEW_ROOT_STATE_EVENT_HPP__
#define __PROCALC_NEW_ROOT_STATE_EVENT_HPP__


#include "event.hpp"


struct NewRootStateEvent : public Event {
  NewRootStateEvent(int stateId)
    : Event("newRootState"),
      rootState(stateId) {}

    int rootState;
};


#endif
