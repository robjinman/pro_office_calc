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
  SpatialSystem& spatialSystem = m_entityManager.system<SpatialSystem>(ComponentKind::C_SPATIAL);
  CVRect& body = dynamic_cast<CVRect&>(spatialSystem.getComponent(entityId()));

  const Point& target = spatialSystem.sg.player->pos();

  double speed = 100.0 / m_frameRate;
  Vec2f v = normalise(target - body.pos) * speed;

  spatialSystem.moveEntity(entityId(), v);
}

//===========================================
// CEnemyBehaviour::handleEvent
//===========================================
void CEnemyBehaviour::handleEvent(const GameEvent& e) {

}
