#ifndef __PROCALC_FRAGMENTS_F_KERNEL_GAME_LOGIC_HPP__
#define __PROCALC_FRAGMENTS_F_KERNEL_GAME_LOGIC_HPP__


#include <string>
#include "raycast/component.hpp"


class EventSystem;
class Event;
class EntityManager;
class EventHandlerSystem;
class GameEvent;


namespace millennium_bug {


class GameLogic {
  public:
    GameLogic(EventSystem& eventSystem, EntityManager& entityManager);
    GameLogic(const GameLogic& cpy) = delete;

    ~GameLogic();

  private:
    void onEntityDestroyed(const GameEvent& event);
    void onEntityChangeZone(const GameEvent& event);
    void setupLarry(EventHandlerSystem& eventHandlerSystem);

    EventSystem& m_eventSystem;
    EntityManager& m_entityManager;

    entityId_t m_entityId = -1;
};


}



#endif
