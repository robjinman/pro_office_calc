#ifndef __PROCALC_RAYCAST_AGENT_SYSTEM_HPP_
#define __PROCALC_RAYCAST_AGENT_SYSTEM_HPP_


#include <memory>
#include <map>
#include "raycast/system.hpp"
#include "raycast/component.hpp"
#include "raycast/geometry.hpp"


struct CAgent : public Component {
  CAgent(entityId_t entityId)
    : Component(entityId, ComponentKind::C_AGENT) {}

  bool hostile = true;

  virtual ~CAgent() override {}
};

typedef std::unique_ptr<CAgent> pCAgent_t;

class EntityManager;

class AgentSystem : public System {
  public:
    AgentSystem(EntityManager& entityManager)
      : m_entityManager(entityManager) {}

    virtual void update() override;
    virtual void handleEvent(const GameEvent& event) override;
    virtual void handleEvent(const GameEvent& event, const std::set<entityId_t>& entities) override;

    virtual void addComponent(pComponent_t component) override;
    virtual bool hasComponent(entityId_t entityId) const override;
    virtual Component& getComponent(entityId_t entityId) const override;
    virtual void removeEntity(entityId_t id) override;

    void navigateTo(entityId_t entityId, const Point& point);

    virtual ~AgentSystem() override {}

  private:
    EntityManager& m_entityManager;
    std::map<entityId_t, pCAgent_t> m_components;
};


#endif
