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

  std::list<EventHandler> broadcastedEventHandlers;
  std::list<EventHandler> targetedEventHandlers;

  virtual ~CEventHandler() {}
};

typedef std::unique_ptr<CEventHandler> pCEventHandler_t;

class EventHandlerSystem : public System {
  public:
    void update() override {}
    void handleEvent(const GameEvent& event) override;
    void handleEvent(const GameEvent& event, const std::set<entityId_t>& entities) override;

    void addComponent(pComponent_t component) override;
    bool hasComponent(entityId_t entityId) const override;
    CEventHandler& getComponent(entityId_t entityId) const override;
    void removeEntity(entityId_t id) override;

  private:
    std::map<entityId_t, pCEventHandler_t> m_components;
};


#endif
