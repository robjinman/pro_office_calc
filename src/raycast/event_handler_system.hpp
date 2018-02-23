#ifndef __PROCALC_RAYCAST_EVENT_HANDLER_SYSTEM_HPP__
#define __PROCALC_RAYCAST_EVENT_HANDLER_SYSTEM_HPP__


#include <functional>
#include <memory>
#include <map>
#include <list>
#include "raycast/system.hpp"
#include "raycast/component.hpp"


struct EventHandler {
  std::string name;
  std::function<void(const GameEvent& event)> handler;
};

struct CEventHandler : public Component {
  explicit CEventHandler(entityId_t entityId)
    : Component(entityId, ComponentKind::C_EVENT_HANDLER) {}

  std::list<EventHandler> handlers;

  virtual ~CEventHandler() {}
};

typedef std::unique_ptr<CEventHandler> pCEventHandler_t;

class EventHandlerSystem : public System {
  public:
    virtual void update() override {}
    virtual void handleEvent(const GameEvent& event) override;

    virtual void addComponent(pComponent_t component) override;
    virtual bool hasComponent(entityId_t entityId) const override;
    virtual Component& getComponent(entityId_t entityId) const override;
    virtual void removeEntity(entityId_t id) override;

    virtual ~EventHandlerSystem() override {}

  private:
    std::map<entityId_t, pCEventHandler_t> m_components;
};


#endif
