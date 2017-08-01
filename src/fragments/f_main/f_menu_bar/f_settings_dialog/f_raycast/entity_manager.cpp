#include <cassert>
#include "fragments/f_main/f_menu_bar/f_settings_dialog/f_raycast/entity_manager.hpp"


entityId_t EntityManager::getNextId() const {
  return Component::getNextId();
}

void EntityManager::addSystem(ComponentKind kind, pSystem_t system) {
  m_systems[kind] = std::move(system);
}

bool EntityManager::hasComponent(entityId_t entityId, ComponentKind kind) const {
  assert(false); // TODO
}

void EntityManager::addComponent(entityId_t entityId, pComponent_t component) {
  assert(false); // TODO
}

void EntityManager::deleteEntity(entityId_t entityId) {
  m_pendingDelete.insert(entityId);
}

void EntityManager::purgeEntities() {
  for (auto it = m_systems.begin(); it != m_systems.end(); ++it) {
    for (auto jt = m_pendingDelete.begin(); jt != m_pendingDelete.end(); ++jt) {
      it->second->removeEntity(*jt);
    }
  }
}

void EntityManager::update() {
  for (auto it = m_systems.begin(); it != m_systems.end(); ++it) {
    System& system = *it->second;
    system.update();
  }
}

void EntityManager::broadcastEvent(const GameEvent& event) const {
  for (auto it = m_systems.begin(); it != m_systems.end(); ++it) {
    System& system = *it->second;
    system.handleEvent(event);
  }
}
