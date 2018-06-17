#ifndef __PROCALC_FRAGMENTS_F_KERNEL_GAME_LOGIC_HPP__
#define __PROCALC_FRAGMENTS_F_KERNEL_GAME_LOGIC_HPP__


#include <string>
#include "raycast/component.hpp"


class EventSystem;
class Event;
class EntityManager;
class EventHandlerSystem;
class GameEvent;
class RootFactory;


namespace millennium_bug {


class ObjectFactory;

class GameLogic {
  public:
    GameLogic(EventSystem& eventSystem, EntityManager& entityManager, RootFactory& rootFactory,
      ObjectFactory& objectFactory);
    GameLogic(const GameLogic& cpy) = delete;

    ~GameLogic();

  private:
    void onEntityDestroyed(const GameEvent& event);
    void onEntityChangeZone(const GameEvent& event);
    void setupLarry(EventHandlerSystem& eventHandlerSystem);

    EventSystem& m_eventSystem;
    EntityManager& m_entityManager;
    RootFactory& m_rootFactory;
    ObjectFactory& m_objectFactory;

    entityId_t m_entityId = -1;
};


}



#endif
