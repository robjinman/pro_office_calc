#ifndef __PROCALC_FRAGMENTS_F_PROCALC_SETUP_EVENTS_HPP__
#define __PROCALC_FRAGMENTS_F_PROCALC_SETUP_EVENTS_HPP__


#include <set>
#include "event.hpp"
#include "button_grid.hpp"


namespace making_progress {


struct SetupCompleteEvent : public Event {
  SetupCompleteEvent(const std::set<buttonId_t>& features)
    : Event("makingProgress/setupComplete"),
      features(features) {}

    std::set<buttonId_t> features;
};

struct ButtonPressEvent : public Event {
  ButtonPressEvent(const std::string& display)
    : Event("makingProgress/buttonPress"),
      calcDisplay(display) {}

    std::string calcDisplay;
};


}


#endif
