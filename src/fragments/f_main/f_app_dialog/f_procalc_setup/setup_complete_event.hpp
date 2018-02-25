#ifndef __PROCALC_FRAGMENTS_F_PROCALC_SETUP_SETUP_COMPLETE_EVENT_HPP__
#define __PROCALC_FRAGMENTS_F_PROCALC_SETUP_SETUP_COMPLETE_EVENT_HPP__


#include <set>
#include "event.hpp"
#include "button_grid.hpp"


struct SetupCompleteEvent : public Event {
  SetupCompleteEvent(const std::set<buttonId_t>& features)
    : Event("makingProgress/setupComplete"),
      features(features) {}

    std::set<buttonId_t> features;
};


#endif
