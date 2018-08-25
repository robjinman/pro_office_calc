#ifndef __PROCALC_RAYCAST_SYSTEM_ACCESSOR_HPP__
#define __PROCALC_RAYCAST_SYSTEM_ACCESSOR_HPP__


#include "raycast/entity_manager.hpp"


class BehaviourSystem;
class SpatialSystem;
class RenderSystem;
class AnimationSystem;
class InventorySystem;
class EventHandlerSystem;
class DamageSystem;
class SpawnSystem;
class AgentSystem;
class FocusSystem;

class SystemAccessor {
  protected:
    SystemAccessor(EntityManager& entityManager);

    BehaviourSystem& behaviourSys() const;
    SpatialSystem& spatialSys() const;
    RenderSystem& renderSys() const;
    AnimationSystem& animationSys() const;
    InventorySystem& inventorySys() const;
    EventHandlerSystem& eventHandlerSys() const;
    DamageSystem& damageSys() const;
    SpawnSystem& spawnSys() const;
    AgentSystem& agentSys() const;
    FocusSystem& focusSys() const;

    virtual ~SystemAccessor() = 0;

  private:
    EntityManager& m_entityManager;
};


#endif
