#ifndef __PROCALC_RAYCAST_FOCUS_SYSTEM_HPP_
#define __PROCALC_RAYCAST_FOCUS_SYSTEM_HPP_


#include <functional>
#include <memory>
#include <map>
#include "raycast/system.hpp"
#include "raycast/component.hpp"


struct CFocus : public Component {
  CFocus(entityId_t entityId)
    : Component(entityId, ComponentKind::C_FOCUS) {}

  std::string hoverText;
  std::string captionText;
  std::function<void()> onHover;
};

typedef std::unique_ptr<CFocus> pCFocus_t;

class EntityManager;
class TimeService;
class AppConfig;

class FocusSystem : public System {
  public:
    FocusSystem(const AppConfig& appConfig, EntityManager& entityManager, TimeService& timeService);

    void update() override;
    void handleEvent(const GameEvent& event) override {}
    void handleEvent(const GameEvent& event, const std::set<entityId_t>& entities) override {}

    void addComponent(pComponent_t component) override;
    bool hasComponent(entityId_t entityId) const override;
    CFocus& getComponent(entityId_t entityId) const override;
    void removeEntity(entityId_t id) override;

    void showCaption(entityId_t entity);

  private:
    void initialise();
    void deleteCaption();

    const AppConfig& m_appConfig;
    EntityManager& m_entityManager;
    TimeService& m_timeService;
    bool m_initialised = false;
    entityId_t m_toolTipId = -1;
    double m_focusDistance = 0;

    entityId_t m_captionBgId = -1;
    entityId_t m_captionTextId = -1;
    long m_captionTimeoutId = -1;

    std::map<entityId_t, pCFocus_t> m_components;
};


#endif
