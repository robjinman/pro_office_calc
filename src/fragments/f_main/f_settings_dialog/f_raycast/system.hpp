#ifndef __PROCALC_FRAGMENTS_F_RAYCAST_SYSTEM_HPP__
#define __PROCALC_FRAGMENTS_F_RAYCAST_SYSTEM_HPP__


#include <memory>
#include <set>
#include "fragments/f_main/f_settings_dialog/f_raycast/component.hpp"


class System {
  public:
    virtual void update() = 0;
    virtual void handleEvent(const GameEvent& event) = 0;
    virtual void handleEvent(const GameEvent& event, const std::set<entityId_t>& entities) = 0;
    virtual void addComponent(pComponent_t component) = 0;
    virtual bool hasComponent(entityId_t entityId) const = 0;
    virtual Component& getComponent(entityId_t entityId) const = 0;
    virtual void removeEntity(entityId_t id) = 0;

    virtual ~System() {}
};

typedef std::unique_ptr<System> pSystem_t;


#endif