#ifndef __PROCALC_RAYCAST_COMPONENT_HPP__
#define __PROCALC_RAYCAST_COMPONENT_HPP__


#include <memory>
#include <string>
#include "raycast/component_kinds.hpp"


typedef long entityId_t;


struct GameEvent {
  explicit GameEvent(const std::string& name)
    : name(name) {}

  std::string name;

  virtual ~GameEvent() {}
};

struct EActivateEntity : public GameEvent {
  EActivateEntity()
    : GameEvent("activate_entity") {}
};

class Component {
  public:
    static entityId_t getNextId();
    static entityId_t getIdFromString(const std::string& s);

    Component(entityId_t entityId, ComponentKind kind)
      : m_entityId(entityId),
        m_kind(kind) {}

    Component(const Component& cpy) = delete;

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
