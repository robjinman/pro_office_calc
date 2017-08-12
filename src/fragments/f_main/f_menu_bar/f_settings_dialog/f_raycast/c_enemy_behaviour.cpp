#include "fragments/f_main/f_menu_bar/f_settings_dialog/f_raycast/c_enemy_behaviour.hpp"
#include "fragments/f_main/f_menu_bar/f_settings_dialog/f_raycast/spatial_system.hpp"
#include "fragments/f_main/f_menu_bar/f_settings_dialog/f_raycast/render_components.hpp"
#include "fragments/f_main/f_menu_bar/f_settings_dialog/f_raycast/entity_manager.hpp"
#include "event.hpp"


//===========================================
// CEnemyBehaviour::CEnemyBehaviour
//===========================================
CEnemyBehaviour::CEnemyBehaviour(entityId_t entityId, EntityManager& entityManager)
  : CBehaviour(entityId),
    m_entityManager(entityManager) {}

//===========================================
// CEnemyBehaviour::update
//===========================================
void CEnemyBehaviour::update() {

}

//===========================================
// CEnemyBehaviour::handleEvent
//===========================================
void CEnemyBehaviour::handleEvent(const GameEvent& e) {

}
