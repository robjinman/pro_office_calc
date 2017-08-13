#ifndef __PROCALC_FRAGMENTS_F_RAYCAST_DAMAGE_SYSTEM_HPP_
#define __PROCALC_FRAGMENTS_F_RAYCAST_DAMAGE_SYSTEM_HPP_


#include <functional>
#include <memory>
#include <map>
#include "fragments/f_main/f_menu_bar/f_settings_dialog/f_raycast/system.hpp"
#include "fragments/f_main/f_menu_bar/f_settings_dialog/f_raycast/component.hpp"
#include "fragments/f_main/f_menu_bar/f_settings_dialog/f_raycast/geometry.hpp"


struct CDamage : public Component {
  CDamage(entityId_t entityId, int maxHealth, int health)
    : Component(entityId, ComponentKind::C_DAMAGE),
      maxHealth(maxHealth),
      health(health) {}

  int maxHealth;
  int health;

  virtual ~CDamage() override {}
};

typedef std::unique_ptr<CDamage> pCDamage_t;

enum class AttenuationCurve {
  CONSTANT,
  LINEAR
};

class EntityManager;

class DamageSystem : public System {
  public:
    DamageSystem(EntityManager& entityManager)
      : m_entityManager(entityManager) {}

    virtual void update() override {}
    virtual void handleEvent(const GameEvent& event) override {}
    virtual void handleEvent(const GameEvent& event,
      const std::set<entityId_t>& entities) override {}

    virtual void addComponent(pComponent_t component) override;
    virtual bool hasComponent(entityId_t entityId) const override;
    virtual Component& getComponent(entityId_t entityId) const override;
    virtual void removeEntity(entityId_t id) override;

    void damageEntity(entityId_t id, double damage);

    void damageWithinRadius(const Point& pos, double radius, int damage,
      AttenuationCurve attenuation = AttenuationCurve::LINEAR);

    void damageAtIntersection(double camSpaceHAngle, double camSpaceVAngle, int damage);

    virtual ~DamageSystem() override {}

  private:
    EntityManager& m_entityManager;
    std::map<entityId_t, pCDamage_t> m_components;
};


#endif
