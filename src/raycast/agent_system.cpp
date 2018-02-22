#include "raycast/agent_system.hpp"
#include "raycast/entity_manager.hpp"
#include "utils.hpp"
#include "exception.hpp"


using std::set;


//===========================================
// AgentSystem::update
//===========================================
void AgentSystem::update() {

}

//===========================================
// AgentSystem::navigateTo
//===========================================
void AgentSystem::navigateTo(entityId_t entityId, const Point& point) {
  DBG_PRINT("Entity " << entityId << " navigating to " << point << "\n");
}

//===========================================
// AgentSystem::handleEvent
//===========================================
void AgentSystem::handleEvent(const GameEvent& event) {

}

//===========================================
// AgentSystem::handleEvent
//===========================================
void AgentSystem::handleEvent(const GameEvent& event, const set<entityId_t>& entities) {

}

//===========================================
// AgentSystem::hasComponent
//===========================================
bool AgentSystem::hasComponent(entityId_t entityId) const {
  return m_components.find(entityId) != m_components.end();
}

//===========================================
// AgentSystem::getComponent
//===========================================
Component& AgentSystem::getComponent(entityId_t entityId) const {
  return *m_components.at(entityId);
}

//===========================================
// AgentSystem::addComponent
//===========================================
void AgentSystem::addComponent(pComponent_t component) {
  if (component->kind() != ComponentKind::C_AGENT) {
    EXCEPTION("Component is not of type CAgent");
  }

  CAgent* p = dynamic_cast<CAgent*>(component.release());
  m_components.insert(std::make_pair(p->entityId(), pCAgent_t(p)));
}

//===========================================
// AgentSystem::removeEntity
//===========================================
void AgentSystem::removeEntity(entityId_t entityId) {
  m_components.erase(entityId);
}
