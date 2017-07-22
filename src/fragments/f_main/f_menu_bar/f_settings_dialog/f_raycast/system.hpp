#ifndef __PROCALC_FRAGMENTS_F_RAYCAST_SYSTEM_HPP__
#define __PROCALC_FRAGMENTS_F_RAYCAST_SYSTEM_HPP__


#include <memory>
#include "fragments/f_main/f_menu_bar/f_settings_dialog/f_raycast/component.hpp"


class Event;

class System {
  public:
    virtual void update() = 0;
    virtual void handleEvent(const Event& event) = 0;
    virtual void addComponent(pComponent_t component) = 0;
    virtual void removeEntity(entityId_t id) = 0;

    virtual ~System() {}
};

typedef std::unique_ptr<System> pSystem_t;


#endif
