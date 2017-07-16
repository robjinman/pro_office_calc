#include "event.hpp"
#include "exception.hpp"
#include "fragments/f_main/f_menu_bar/f_settings_dialog/f_raycast/behaviour_system.hpp"


//===========================================
// BehaviourSystem::update
//===========================================
void BehaviourSystem::update() {
  for (auto it = m_components.begin(); it != m_components.end(); ++it) {
    it->second->fnUpdate();
  }
}

//===========================================
// BehaviourSystem::handleEvent
//===========================================
void BehaviourSystem::handleEvent(const Event& event) {
  for (auto it = m_components.begin(); it != m_components.end(); ++it) {
    it->second->fnHandleEvent(event);
  }
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
// BehaviourSystem::removeComponent
//===========================================
void BehaviourSystem::removeComponent(entityId_t entityId) {
  m_components.erase(entityId);
}
