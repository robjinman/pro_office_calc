#ifndef __PROCALC_RAYCAST_FOCUS_SYSTEM_HPP_
#define __PROCALC_RAYCAST_FOCUS_SYSTEM_HPP_


#include <memory>
#include <map>
#include "raycast/system.hpp"
#include "raycast/component.hpp"


struct CFocus : public Component {
  CFocus(entityId_t entityId)
    : Component(entityId, ComponentKind::C_FOCUS) {}

  std::string tooltip;
  std::function<void()> onFocus;
};

typedef std::unique_ptr<CFocus> pCFocus_t;

class EntityManager;
class RenderSystem;

class FocusSystem : public System {
  public:
    FocusSystem(EntityManager& entityManager);

    void update() override;
    void handleEvent(const GameEvent& event) override {}
    void handleEvent(const GameEvent& event, const std::set<entityId_t>& entities) override {}

    void addComponent(pComponent_t component) override;
    bool hasComponent(entityId_t entityId) const override;
    CFocus& getComponent(entityId_t entityId) const override;
    void removeEntity(entityId_t id) override;

  private:
    void initialise();

    EntityManager& m_entityManager;
    bool m_initialised = false;
    entityId_t m_tooltipId = -1;

    std::map<entityId_t, pCFocus_t> m_components;
};


#endif
