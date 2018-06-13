#ifndef __PROCALC_RAYCAST_GAME_EVENT_HPP_
#define __PROCALC_RAYCAST_GAME_EVENT_HPP_


#include <string>


class GameEvent {
  public:
    explicit GameEvent(const std::string& name)
      : name(name) {}

    std::string name;

    virtual ~GameEvent() {}
};

class EActivateEntity : public GameEvent {
  public:
    EActivateEntity()
      : GameEvent("activate_entity") {}
};

struct EMouseCaptured : public GameEvent {
  EMouseCaptured()
    : GameEvent("mouse_captured") {}
};

struct EMouseUncaptured : public GameEvent {
  EMouseUncaptured()
    : GameEvent("mouse_uncaptured") {}
};


#endif
