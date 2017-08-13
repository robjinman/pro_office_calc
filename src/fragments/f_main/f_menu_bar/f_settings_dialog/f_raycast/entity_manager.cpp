#include <cassert>
#include "fragments/f_main/f_menu_bar/f_settings_dialog/f_raycast/entity_manager.hpp"


using std::set;


//===========================================
// EntityManager::getNextId
//===========================================
entityId_t EntityManager::getNextId() const {
  return Component::getNextId();
}

//===========================================
// EntityManager::addSystem
//===========================================
void EntityManager::addSystem(ComponentKind kind, pSystem_t system) {
  m_systems[kind] = std::move(system);
}

//===========================================
// EntityManager::hasComponent
//===========================================
bool EntityManager::hasComponent(entityId_t entityId, ComponentKind kind) const {
  auto it = m_systems.find(kind);
  if (it == m_systems.end()) {
    return false;
  }

  System& system = *it->second;
  return system.hasComponent(entityId);
}

//===========================================
// EntityManager::addComponent
//===========================================
void EntityManager::addComponent(pComponent_t component) {
  System& system = *m_systems.at(component->kind());
  system.addComponent(std::move(component));
}

//===========================================
// EntityManager::deleteEntity
//===========================================
void EntityManager::deleteEntity(entityId_t entityId) {
  m_pendingDelete.insert(entityId);
}

//===========================================
// EntityManager::purgeEntities
//===========================================
void EntityManager::purgeEntities() {
  for (auto it = m_systems.begin(); it != m_systems.end(); ++it) {
    for (auto jt = m_pendingDelete.begin(); jt != m_pendingDelete.end(); ++jt) {
      it->second->removeEntity(*jt);
    }
  }
}

//===========================================
// EntityManager::update
//===========================================
void EntityManager::update() {
  for (auto it = m_systems.begin(); it != m_systems.end(); ++it) {
    System& system = *it->second;
    system.update();
  }
}

//===========================================
// EntityManager::broadcastEvent
//===========================================
void EntityManager::broadcastEvent(const GameEvent& event) const {
  for (auto it = m_systems.begin(); it != m_systems.end(); ++it) {
    System& system = *it->second;
    system.handleEvent(event);
  }
}

//===========================================
// EntityManager::broadcastEvent
//===========================================
void EntityManager::broadcastEvent(const GameEvent& event, const set<entityId_t>& entities) const {
  for (auto it = m_systems.begin(); it != m_systems.end(); ++it) {
    System& system = *it->second;
    system.handleEvent(event, entities);
  }
}
