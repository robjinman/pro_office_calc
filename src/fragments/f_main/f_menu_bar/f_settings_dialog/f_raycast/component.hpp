#ifndef __PROCALC_FRAGMENTS_F_RAYCAST_COMPONENT_HPP__
#define __PROCALC_FRAGMENTS_F_RAYCAST_COMPONENT_HPP__


#include <memory>
#include <set>
#include "fragments/f_main/f_menu_bar/f_settings_dialog/f_raycast/component_kinds.hpp"


typedef long entityId_t;


struct GameEvent {
  GameEvent(const std::string& name)
    : name(name) {}

  std::string name;
  std::set<entityId_t> entitiesInRange;

  virtual ~GameEvent() {}
};

class Component {
  public:
    static entityId_t getNextId();

    Component(entityId_t entityId, ComponentKind kind)
      : m_entityId(entityId),
        m_kind(kind) {}

    Component(const Component& cpy)
      : m_entityId(cpy.m_entityId),
        m_kind(cpy.m_kind) {}

    entityId_t entityId() const {
      return m_entityId;
    }

    ComponentKind kind() const {
      return m_kind;
    }

    virtual ~Component() {}

  private:
    static entityId_t nextId;

    entityId_t m_entityId;
    ComponentKind m_kind;
};

typedef std::unique_ptr<Component> pComponent_t;


#endif
