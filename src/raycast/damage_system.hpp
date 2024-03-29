#ifndef __PROCALC_RAYCAST_DAMAGE_SYSTEM_HPP_
#define __PROCALC_RAYCAST_DAMAGE_SYSTEM_HPP_


#include <functional>
#include <memory>
#include <map>
#include "raycast/system.hpp"
#include "raycast/component.hpp"
#include "raycast/geometry.hpp"
#include "raycast/system_accessor.hpp"


struct EEntityDestroyed : public GameEvent {
  EEntityDestroyed(entityId_t id)
    : GameEvent("entity_destroyed"),
      entityId(id) {}

  entityId_t entityId;

  Point point_wld;
  Point point_rel;
};

struct EEntityDamaged : public GameEvent {
  EEntityDamaged(entityId_t id, int health, int prevHealth)
    : GameEvent("entity_damaged"),
      entityId(id),
      health(health),
      prevHealth(prevHealth) {}

  entityId_t entityId;

  Point point_wld;
  Point point_rel;
  int health;
  int prevHealth;
};

struct CDamage : public Component {
  CDamage(entityId_t entityId, int maxHealth, int health)
    : Component(entityId, ComponentKind::C_DAMAGE),
      maxHealth(maxHealth),
      health(smallest(health, maxHealth)) {}

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
class CZone;
struct Intersection;

class DamageSystem : public System, private SystemAccessor {
  public:
    DamageSystem(EntityManager& entityManager)
      : SystemAccessor(entityManager),
        m_entityManager(entityManager) {}

    void update() override {}
    void handleEvent(const GameEvent&) override {}
    void handleEvent(const GameEvent&, const std::set<entityId_t>&) override {}

    void addComponent(pComponent_t component) override;
    bool hasComponent(entityId_t entityId) const override;
    CDamage& getComponent(entityId_t entityId) const override;
    void removeEntity(entityId_t id) override;

    void damageEntity(entityId_t id, int damage);

    void damageWithinRadius(const CZone& zone, const Point& pos, double radius, int damage,
      AttenuationCurve attenuation = AttenuationCurve::LINEAR);

    void damageAtIntersection(const Vec2f& ray, double camSpaceVAngle, int damage);

    void damageAtIntersection(const CZone& zone, const Point& pos, double height, const Vec2f& dir,
      double vAngle, const Matrix& matrix, int damage);

    int getHealth(entityId_t entityId) const;
    int getMaxHealth(entityId_t entityId) const;

  private:
    EntityManager& m_entityManager;
    std::map<entityId_t, pCDamage_t> m_components;

    void damageAtIntersection_(const Intersection& X, int damage);
};


#endif
