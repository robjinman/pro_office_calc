#include "event.hpp"
#include "exception.hpp"
#include "fragments/f_main/f_menu_bar/f_settings_dialog/f_raycast/behaviour_system.hpp"


//===========================================
// BehaviourSystem::update
//===========================================
void BehaviourSystem::update() {
  for (auto it = m_components.begin(); it != m_components.end(); ++it) {
    it->second->update();
  }
}

//===========================================
// BehaviourSystem::handleEvent
//===========================================
void BehaviourSystem::handleEvent(const GameEvent& event) {
  for (auto it = m_components.begin(); it != m_components.end(); ++it) {
    CBehaviour& c = *it->second;
    if (event.entitiesInRange.count(c.entityId()) == 1) {
      c.handleEvent(event);
    }
  }
}

//===========================================
// BehaviourSystem::getComponent
//===========================================
Component& BehaviourSystem::getComponent(entityId_t entityId) const {
  return *m_components.at(entityId);
}

//===========================================
// BehaviourSystem::addComponent
//===========================================
void BehaviourSystem::addComponent(pComponent_t component) {
  if (component->kind() != ComponentKind::C_BEHAVIOUR) {
    EXCEPTION("Component is not of type CBehaviour");
  }

  CBehaviour* p = dynamic_cast<CBehaviour*>(component.release());
  m_components.insert(std::make_pair(p->entityId(), pCBehaviour_t(p)));
}

//===========================================
// BehaviourSystem::removeEntity
//===========================================
void BehaviourSystem::removeEntity(entityId_t entityId) {
  m_components.erase(entityId);
}