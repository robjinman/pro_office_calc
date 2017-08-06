#include "fragments/f_main/f_menu_bar/f_settings_dialog/f_raycast/damage_system.hpp"
#include "fragments/f_main/f_menu_bar/f_settings_dialog/f_raycast/spatial_system.hpp"
#include "fragments/f_main/f_menu_bar/f_settings_dialog/f_raycast/event_handler_system.hpp"
#include "fragments/f_main/f_menu_bar/f_settings_dialog/f_raycast/entity_manager.hpp"


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
    CDamage& component = *it->second;

    EventHandlerSystem& eventHandlerSystem = m_entityManager
      .system<EventHandlerSystem>(ComponentKind::C_EVENT_HANDLER);

    if (component.health > 0) {
      component.health -= damage;
      eventHandlerSystem.sendEvent(id, GameEvent("entityDamaged"));

      if (component.health <= 0) {
        component.health = 0;
        eventHandlerSystem.sendEvent(id, GameEvent("entityDestroyed"));
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
void DamageSystem::damageAtIntersection(double camSpaceHAngle, double camSpaceVAngle, int damage) {
  SpatialSystem& spatialSystem = m_entityManager.system<SpatialSystem>(ComponentKind::C_SPATIAL);
  list<pIntersection_t> intersections = spatialSystem.entitiesAlong3dRay(camSpaceHAngle,
    camSpaceVAngle);

  if (intersections.size() > 0) {
    damageEntity(intersections.front()->entityId, damage);
  }
}
