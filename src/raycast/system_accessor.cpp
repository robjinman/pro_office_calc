#include "raycast/system_accessor.hpp"
#include "raycast/spatial_system.hpp"
#include "raycast/behaviour_system.hpp"
#include "raycast/render_system.hpp"
#include "raycast/animation_system.hpp"
#include "raycast/inventory_system.hpp"
#include "raycast/event_handler_system.hpp"
#include "raycast/damage_system.hpp"
#include "raycast/spawn_system.hpp"
#include "raycast/agent_system.hpp"
#include "raycast/focus_system.hpp"


SystemAccessor::SystemAccessor(EntityManager& entityManager)
  : m_entityManager(entityManager) {}

BehaviourSystem& SystemAccessor::behaviourSys() const {
  return m_entityManager.system<BehaviourSystem>(ComponentKind::C_BEHAVIOUR);
}

SpatialSystem& SystemAccessor::spatialSys() const {
  return m_entityManager.system<SpatialSystem>(ComponentKind::C_SPATIAL);
}

RenderSystem& SystemAccessor::renderSys() const {
  return m_entityManager.system<RenderSystem>(ComponentKind::C_RENDER);
}

AnimationSystem& SystemAccessor::animationSys() const {
  return m_entityManager.system<AnimationSystem>(ComponentKind::C_ANIMATION);
}

InventorySystem& SystemAccessor::inventorySys() const {
  return m_entityManager.system<InventorySystem>(ComponentKind::C_INVENTORY);
}

EventHandlerSystem& SystemAccessor::eventHandlerSys() const {
  return m_entityManager.system<EventHandlerSystem>(ComponentKind::C_EVENT_HANDLER);
}

DamageSystem& SystemAccessor::damageSys() const {
  return m_entityManager.system<DamageSystem>(ComponentKind::C_DAMAGE);
}

SpawnSystem& SystemAccessor::spawnSys() const {
  return m_entityManager.system<SpawnSystem>(ComponentKind::C_SPAWN);
}

AgentSystem& SystemAccessor::agentSys() const {
  return m_entityManager.system<AgentSystem>(ComponentKind::C_AGENT);
}

FocusSystem& SystemAccessor::focusSys() const {
  return m_entityManager.system<FocusSystem>(ComponentKind::C_FOCUS);
}

SystemAccessor::~SystemAccessor() {}
