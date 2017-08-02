#include "fragments/f_main/f_menu_bar/f_settings_dialog/f_raycast/event_handler_system.hpp"


//===========================================
// EventHandlerSystem::addComponent
//===========================================
void EventHandlerSystem::addComponent(pComponent_t component) {
  pCEventHandler_t c(dynamic_cast<CEventHandler*>(component.release()));
  m_components.insert(make_pair(c->entityId(), std::move(c)));
}

//===========================================
// EventHandlerSystem::hasComponent
//===========================================
bool EventHandlerSystem::hasComponent(entityId_t entityId) const {
  return m_components.find(entityId) != m_components.end();
}

//===========================================
// EventHandlerSystem::getComponent
//===========================================
Component& EventHandlerSystem::getComponent(entityId_t entityId) const {
  return *m_components.at(entityId);
}

//===========================================
// EventHandlerSystem::removeEntity
//===========================================
void EventHandlerSystem::removeEntity(entityId_t id) {
  m_components.erase(id);
}

//===========================================
// EventHandlerSystem::sendEvent
//===========================================
void EventHandlerSystem::sendEvent(entityId_t entityId, const GameEvent& event) {
  auto it = m_components.find(entityId);
  if (it != m_components.end()) {
    CEventHandler& c = *it->second;

    for (auto jt = c.handlers.begin(); jt != c.handlers.end(); ++jt) {
      EventHandler handler = *jt;
      if (handler.name == event.name) {
        handler.handler(event);
      }
    }
  }
}
