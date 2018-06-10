#include "raycast/damage_system.hpp"
#include "raycast/spatial_system.hpp"
#include "raycast/entity_manager.hpp"


using std::vector;
using std::set;
using std::make_pair;


const double PENETRATION_DISTANCE = 0.01;


//===========================================
// DamageSystem::addComponent
//===========================================
void DamageSystem::addComponent(pComponent_t component) {
  pCDamage_t c(dynamic_cast<CDamage*>(component.release()));
  m_components.insert(make_pair(c->entityId(), std::move(c)));
}

//===========================================
// DamageSystem::hasComponent
//===========================================
bool DamageSystem::hasComponent(entityId_t entityId) const {
  return m_components.find(entityId) != m_components.end();
}

//===========================================
// DamageSystem::getComponent
//===========================================
CDamage& DamageSystem::getComponent(entityId_t entityId) const {
  return *m_components.at(entityId);
}

//===========================================
// DamageSystem::removeEntity
//===========================================
void DamageSystem::removeEntity(entityId_t id) {
  m_components.erase(id);
}

//===========================================
// fireDamagedEvents
//===========================================
static void fireDamagedEvents(const EntityManager& entityManager, entityId_t id,
  const Point* pt_rel = nullptr, const Point* pt_wld = nullptr) {

  EEntityDamaged damaged(id);

  if (pt_rel != nullptr) {
    damaged.point_rel = *pt_rel;
  }

  if (pt_wld != nullptr) {
    damaged.point_wld = *pt_wld;
  }

  entityManager.fireEvent(damaged, {id});
  entityManager.broadcastEvent(EEntityDamaged(id));
}

//===========================================
// fireDestroyedEvents
//===========================================
static void fireDestroyedEvents(const EntityManager& entityManager, entityId_t id,
  const Point* pt_rel = nullptr, const Point* pt_wld = nullptr) {

  EEntityDestroyed destroyed(id);

  if (pt_rel != nullptr) {
    destroyed.point_rel = *pt_rel;
  }

  if (pt_wld != nullptr) {
    destroyed.point_wld = *pt_wld;
  }

  entityManager.fireEvent(destroyed, {id});
  entityManager.broadcastEvent(EEntityDestroyed(id));
}

//===========================================
// DamageSystem::damageEntity
//===========================================
void DamageSystem::damageEntity(entityId_t entityId, double damage) {
  const SpatialSystem& spatialSystem =
    m_entityManager.system<SpatialSystem>(ComponentKind::C_SPATIAL);

  set<entityId_t> entities = spatialSystem.getAncestors(entityId);
  entities.insert(entityId);

  for (entityId_t id : entities) {
    auto it = m_components.find(id);
    if (it != m_components.end()) {
      DBG_PRINT("Damaging entity " << id << "\n");
      CDamage& component = *it->second;

      if (component.health > 0 || damage < 0) {
        component.health -= damage;

        if (component.health < 0) {
          component.health = 0;
        }

        fireDamagedEvents(m_entityManager, id);

        if (component.health == 0) {
          fireDestroyedEvents(m_entityManager, id);
        }
      }
    }
  }
}

//===========================================
// DamageSystem::damageWithinRadius
//===========================================
void DamageSystem::damageWithinRadius(const CZone& zone, const Point& pos, double radius,
  int damage, AttenuationCurve attenuation) {

  // TODO: Attenuation

  SpatialSystem& spatialSystem = m_entityManager.system<SpatialSystem>(ComponentKind::C_SPATIAL);
  set<entityId_t> entities = spatialSystem.entitiesInRadius(zone, pos, radius);

  for (auto it = entities.begin(); it != entities.end(); ++it) {
    damageEntity(*it, damage);
  }
}

//===========================================
// DamageSystem::damageAtIntersection_
//===========================================
void DamageSystem::damageAtIntersection_(const Intersection& X, int damage) {
  const SpatialSystem& spatialSystem =
    m_entityManager.system<SpatialSystem>(ComponentKind::C_SPATIAL);

  set<entityId_t> entities = spatialSystem.getAncestors(X.entityId);
  entities.insert(X.entityId);

  for (entityId_t id : entities) {
    auto it = m_components.find(id);
    if (it != m_components.end()) {
      CDamage& component = *it->second;

      if (component.health > 0) {
        component.health -= damage;

        if (component.health < 0) {
          component.health = 0;
        }

        Point pt_rel(X.distanceAlongTarget, X.height);
        fireDamagedEvents(m_entityManager, id, &pt_rel, &X.point_wld);

        if (component.health == 0) {
          Point pt_rel(X.distanceAlongTarget, X.height);
          fireDestroyedEvents(m_entityManager, id, &pt_rel, &X.point_wld);
        }
      }
    }
  }
}

//===========================================
// DamageSystem::damageAtIntersection
//===========================================
void DamageSystem::damageAtIntersection(const Vec2f& ray, double camSpaceVAngle, int damage) {
  SpatialSystem& spatialSystem = m_entityManager.system<SpatialSystem>(ComponentKind::C_SPATIAL);
  vector<pIntersection_t> intersections = spatialSystem.entitiesAlong3dRay(ray, camSpaceVAngle);

  if (intersections.size() > 0) {
    double dist = intersections.front()->distanceFromOrigin;

    for (auto& i : intersections) {
      if (i->distanceFromOrigin <= dist + PENETRATION_DISTANCE) {
        damageAtIntersection_(*i, damage);
      }
      else {
        break;
      }
    }
  }
}

//===========================================
// DamageSystem::damageAtIntersection
//===========================================
void DamageSystem::damageAtIntersection(const CZone& zone, const Point& pos, double height,
  const Vec2f& dir, double vAngle, const Matrix& matrix, int damage) {

  SpatialSystem& spatialSystem = m_entityManager.system<SpatialSystem>(ComponentKind::C_SPATIAL);
  vector<pIntersection_t> intersections = spatialSystem.entitiesAlong3dRay(zone, pos, height, dir,
    vAngle, matrix);

  if (intersections.size() > 0) {
    double dist = intersections.front()->distanceFromOrigin;

    for (auto& i : intersections) {
      if (i->distanceFromOrigin <= dist + PENETRATION_DISTANCE) {
        damageAtIntersection_(*i, damage);
      }
      else {
        break;
      }
    }
  }
}
