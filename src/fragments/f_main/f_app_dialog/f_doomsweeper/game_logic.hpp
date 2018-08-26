#ifndef __PROCALC_FRAGMENTS_F_DOOMSWEEPER_GAME_LOGIC_HPP__
#define __PROCALC_FRAGMENTS_F_DOOMSWEEPER_GAME_LOGIC_HPP__


#include <string>
#include <future>
#include <atomic>
#include <stdexcept>
#include <map>
#include <list>
#include <set>
#include "raycast/component.hpp"
#include "raycast/system_accessor.hpp"
#include "event_system.hpp"
#include "fragments/f_main/f_app_dialog/f_minesweeper/events.hpp"


class Event;
class EntityManager;
class EventHandlerSystem;
class GameEvent;
class RootFactory;
class TimeService;
class CZone;


namespace doomsweeper {


class ObjectFactory;

template<class T, int SIZE>
class FixedSizeList {
  public:
    const T& oldest() const {
      return m_list.front();
    }

    const T& newest() const {
      return m_list.back();
    }

    const T& nthNewest(unsigned int n) const {
      if (n >= m_list.size()) {
        throw std::out_of_range("Index out of range");
      }

      unsigned int i = 0;
      for (const T& e : m_list) {
        if (n + 1 + i == m_list.size()) {
          return e;
        }

        ++i;
      }

      throw std::out_of_range("Index out of range");
    }

    void push(const T& element) {
      m_list.push_back(element);

      if (m_list.size() > SIZE) {
        m_list.pop_front();
      }
    }

    int size() const {
      return static_cast<int>(m_list.size());
    }

  private:
    std::list<T> m_list;
};

class GameLogic : private SystemAccessor {
  public:
    GameLogic(EventSystem& eventSystem, EntityManager& entityManager, RootFactory& rootFactory,
      ObjectFactory& objectFactory, TimeService& timeService);
    GameLogic(const GameLogic& cpy) = delete;

    std::future<void> initialise(const std::set<Coord>& mineCoords);

    ~GameLogic();

  private:
    void onPlayerEnterCellInner(entityId_t cellId);
    void onCellDoorOpened(entityId_t cellId);
    void onEntityChangeZone(const GameEvent& e_);
    void onClickMine(const Event&);
    void onCommandsEntered(const Event&);
    void onDoomWindowClose();
    void lockDoors();
    void sealDoor(entityId_t doorId);
    void drawMazeMap(const std::set<Coord>& clueCells);
    void generateCommands();
    void drawCommandScreens(const std::vector<std::vector<std::string>>& commands) const;

    std::atomic<bool> m_initialised;

    EventSystem& m_eventSystem;
    EntityManager& m_entityManager;
    RootFactory& m_rootFactory;
    ObjectFactory& m_objectFactory;
    TimeService& m_timeService;

    entityId_t m_entityId = -1;

    EventHandle m_hClickMine;
    EventHandle m_hCommandsEntered;
    EventHandle m_hDoomClosed;

    std::map<entityId_t, Coord> m_cellIds;

    FixedSizeList<entityId_t, 3> m_history;
};


}



#endif
