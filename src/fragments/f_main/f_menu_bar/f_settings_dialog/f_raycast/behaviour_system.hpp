#ifndef __PROCALC_FRAGMENTS_F_RAYCAST_BEHAVIOUR_SYSTEM_HPP_
#define __PROCALC_FRAGMENTS_F_RAYCAST_BEHAVIOUR_SYSTEM_HPP_


#include <functional>
#include <memory>
#include <map>
#include "fragments/f_main/f_menu_bar/f_settings_dialog/f_raycast/system.hpp"
#include "fragments/f_main/f_menu_bar/f_settings_dialog/f_raycast/component.hpp"


class CBehaviour;

typedef std::unique_ptr<CBehaviour> pCBehaviour_t;

class BehaviourSystem : public System {
  public:
    virtual void update() override;
    virtual void handleEvent(const GameEvent& event) override;

    virtual void addComponent(pComponent_t component) override;
    virtual Component& getComponent(entityId_t entityId) const override;
    virtual void removeEntity(entityId_t id) override;

    virtual ~BehaviourSystem() override {}

  private:
    std::map<entityId_t, pCBehaviour_t> m_components;
};

struct CBehaviour : public Component {
  CBehaviour(entityId_t entityId)
    : Component(entityId, ComponentKind::C_BEHAVIOUR) {}

  virtual void update() = 0;
  virtual void handleEvent(const GameEvent& e) = 0;

  virtual ~CBehaviour() override {}
};


#endif
