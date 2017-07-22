#include "fragments/f_main/f_menu_bar/f_settings_dialog/f_raycast/entity_manager.hpp"


entityId_t EntityManager::getNextId() const {
  return Component::getNextId();
}

void EntityManager::addSystem(ComponentKind kind, pSystem_t system) {
  m_systems[kind] = std::move(system);
}

bool EntityManager::hasComponent(entityId_t entityId, ComponentKind kind) const {

}

Component& EntityManager::getComponent(entityId_t entityId) const {

}

void EntityManager::addComponent(entityId_t entityId, pComponent_t component) {

}

void EntityManager::deleteEntity(entityId_t entityId) {

}

void EntityManager::sendEventToEntity(entityId_t entityId, const Event& event) const {

}
