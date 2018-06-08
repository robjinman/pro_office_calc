#ifndef __PROCALC_RAYCAST_ENTITY_MANAGER_HPP_
#define __PROCALC_RAYCAST_ENTITY_MANAGER_HPP_


#include <map>
#include <set>
#include "raycast/component.hpp"
#include "raycast/system.hpp"


class EEntityDeleted : public GameEvent {
  public:
    EEntityDeleted(entityId_t entityId)
      : GameEvent("entity_deleted"),
        entityId(entityId) {}

    entityId_t entityId;
};

class EntityManager {
  public:
    entityId_t getNextId() const;

    void addSystem(ComponentKind kind, pSystem_t system);
    bool hasComponent(entityId_t entityId, ComponentKind kind) const;
    void addComponent(pComponent_t component);
    void deleteEntity(entityId_t entityId);
    void purgeEntities();
    void broadcastEvent(const GameEvent& event) const;
    void fireEvent(const GameEvent& event, const std::set<entityId_t>& entities) const;
    void update();

    template<class T>
    T& getComponent(entityId_t entityId, ComponentKind kind) const {
      System& sys = *m_systems.at(kind);
      Component& c = sys.getComponent(entityId);

      return dynamic_cast<T&>(c);
    }

    template<class T>
    T& system(ComponentKind kind) const {
      return dynamic_cast<T&>(*m_systems.at(kind));
    }

  private:
    std::map<ComponentKind, pSystem_t> m_systems;
    std::set<entityId_t> m_pendingDelete;
};


#endif
