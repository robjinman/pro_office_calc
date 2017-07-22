#ifndef __PROCALC_FRAGMENTS_F_RAYCAST_ENTITY_MANAGER_HPP_
#define __PROCALC_FRAGMENTS_F_RAYCAST_ENTITY_MANAGER_HPP_


#include <map>
#include "fragments/f_main/f_menu_bar/f_settings_dialog/f_raycast/component_kinds.hpp"
#include "fragments/f_main/f_menu_bar/f_settings_dialog/f_raycast/system.hpp"


class EntityManager {
  public:
    entityId_t getNextId() const;

    void addSystem(ComponentKind kind, pSystem_t system);

    bool hasComponent(entityId_t entityId, ComponentKind kind) const;
    Component& getComponent(entityId_t entityId) const;
    void addComponent(entityId_t entityId, pComponent_t component);
    void deleteEntity(entityId_t entityId);
    void sendEventToEntity(entityId_t entityId, const Event& event) const;

  private:
    std::map<ComponentKind, pSystem_t> m_systems;
};


#endif
