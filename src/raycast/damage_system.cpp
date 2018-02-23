#include "raycast/damage_system.hpp"
#include "raycast/spatial_system.hpp"
#include "raycast/entity_manager.hpp"


using std::list;
using std::set;
using std::make_pair;


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
Component& DamageSystem::getComponent(entityId_t entityId) const {
  return *m_components.at(entityId);
}

//===========================================
// DamageSystem::removeEntity
//===========================================
void DamageSystem::removeEntity(entityId_t id) {
  m_components.erase(id);
}

//===========================================
// DamageSystem::damageEntity
//===========================================
void DamageSystem::damageEntity(entityId_t id, double damage) {
  auto it = m_components.find(id);

  if (it != m_components.end()) {
    DBG_PRINT("Damaging entity " << id << "\n");
    CDamage& component = *it->second;

    if (component.health > 0) {
      component.health -= damage;
      m_entityManager.broadcastEvent(EEntityDamaged(id));

      if (component.health <= 0) {
        component.health = 0;

        m_entityManager.broadcastEvent(EEntityDestroyed(id));
      }
    }
  }
}

//===========================================
// DamageSystem::damageWithinRadius
//===========================================
void DamageSystem::damageWithinRadius(const Point& pos, double radius, int damage,
  AttenuationCurve attenuation) {

  // TODO: Attenuation

  SpatialSystem& spatialSystem = m_entityManager.system<SpatialSystem>(ComponentKind::C_SPATIAL);
  set<entityId_t> entities = spatialSystem.entitiesInRadius(pos, radius);

  for (auto it = entities.begin(); it != entities.end(); ++it) {
    damageEntity(*it, damage);
  }
}

//===========================================
// DamageSystem::damageAtIntersection
//===========================================
void DamageSystem::damageAtIntersection(const Vec2f& ray, double camSpaceVAngle, int damage) {
  SpatialSystem& spatialSystem = m_entityManager.system<SpatialSystem>(ComponentKind::C_SPATIAL);
  list<pIntersection_t> intersections = spatialSystem.entitiesAlong3dRay(ray, camSpaceVAngle);

  if (intersections.size() > 0) {
    damageEntity(intersections.front()->entityId, damage);
  }
}

//===========================================
// DamageSystem::damageAtIntersection
//===========================================
void DamageSystem::damageAtIntersection(const CZone& zone, const Point& pos, double height,
  const Vec2f& dir, double vAngle, const Matrix& matrix, int damage) {

  SpatialSystem& spatialSystem = m_entityManager.system<SpatialSystem>(ComponentKind::C_SPATIAL);
  list<pIntersection_t> intersections = spatialSystem.entitiesAlong3dRay(zone, pos, height, dir,
    vAngle, matrix);

  if (intersections.size() > 0) {
    damageEntity(intersections.front()->entityId, damage);
  }
}
