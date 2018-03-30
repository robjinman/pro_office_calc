#ifndef __PROCALC_RAYCAST_BEHAVIOUR_SYSTEM_HPP_
#define __PROCALC_RAYCAST_BEHAVIOUR_SYSTEM_HPP_


#include <functional>
#include <memory>
#include <map>
#include "raycast/system.hpp"
#include "raycast/component.hpp"


struct CBehaviour : public Component {
  CBehaviour(entityId_t entityId)
    : Component(entityId, ComponentKind::C_BEHAVIOUR) {}

  virtual void update() = 0;
  virtual void handleBroadcastedEvent(const GameEvent& event) = 0;
  virtual void handleTargetedEvent(const GameEvent& event) = 0;
};

typedef std::unique_ptr<CBehaviour> pCBehaviour_t;

class BehaviourSystem : public System {
  public:
    void update() override;
    void handleEvent(const GameEvent& event) override;
    void handleEvent(const GameEvent& event, const std::set<entityId_t>& entities) override;

    void addComponent(pComponent_t component) override;
    bool hasComponent(entityId_t entityId) const override;
    CBehaviour& getComponent(entityId_t entityId) const override;
    void removeEntity(entityId_t id) override;

  private:
    std::map<entityId_t, pCBehaviour_t> m_components;
};


#endif
