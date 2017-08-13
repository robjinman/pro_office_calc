#include "fragments/f_main/f_menu_bar/f_settings_dialog/f_raycast/c_enemy_behaviour.hpp"
#include "fragments/f_main/f_menu_bar/f_settings_dialog/f_raycast/spatial_system.hpp"
#include "fragments/f_main/f_menu_bar/f_settings_dialog/f_raycast/render_components.hpp"
#include "fragments/f_main/f_menu_bar/f_settings_dialog/f_raycast/entity_manager.hpp"
#include "event.hpp"


//===========================================
// CEnemyBehaviour::CEnemyBehaviour
//===========================================
CEnemyBehaviour::CEnemyBehaviour(entityId_t entityId, EntityManager& entityManager,
  double frameRate)
  : CBehaviour(entityId),
    m_entityManager(entityManager),
    m_frameRate(frameRate) {}

//===========================================
// CEnemyBehaviour::update
//===========================================
void CEnemyBehaviour::update() {
  if (m_state == ST_CHASING) {
    SpatialSystem& spatialSystem = m_entityManager.system<SpatialSystem>(ComponentKind::C_SPATIAL);
    CVRect& body = dynamic_cast<CVRect&>(spatialSystem.getComponent(entityId()));

    const Point& target = spatialSystem.sg.player->pos();

    double speed = 50.0 / m_frameRate;
    Vec2f v = normalise(target - body.pos) * speed;
    body.angle = atan2(v.y, v.x);

    spatialSystem.moveEntity(entityId(), v);
  }
  else if (m_state == ST_PATROLLING) {

  }
}

//===========================================
// CEnemyBehaviour::handleEvent
//===========================================
void CEnemyBehaviour::handleEvent(const GameEvent& e) {

}
