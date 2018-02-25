#include "raycast/event_handler_system.hpp"


using std::set;


//===========================================
// sendEventToComponent
//===========================================
static void sendEventToComponent(CEventHandler& c, const GameEvent& e) {
  for (auto it = c.handlers.begin(); it != c.handlers.end(); ++it) {
    EventHandler handler = *it;

    if (handler.name == e.name || handler.name == "*") {
      handler.handler(e);
    }
  }
}

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
// EventHandlerSystem::handleEvent
//===========================================
void EventHandlerSystem::handleEvent(const GameEvent& event) {
  for (auto it = m_components.begin(); it != m_components.end(); ++it) {
    sendEventToComponent(*it->second, event);
  }
}

//===========================================
// EventHandlerSystem::handleEvent
//===========================================
void EventHandlerSystem::handleEvent(const GameEvent& event, const set<entityId_t>& entities) {
  for (auto it = m_components.begin(); it != m_components.end(); ++it) {
    if (entities.count(it->first)) {
      sendEventToComponent(*it->second, event);
    }
  }
}
