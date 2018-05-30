#include <random>
#include "fragments/f_main/f_app_dialog/f_file_system/game_logic.hpp"
#include "raycast/entity_manager.hpp"
#include "raycast/spatial_system.hpp"
#include "raycast/event_handler_system.hpp"
#include "raycast/render_system.hpp"
#include "raycast/damage_system.hpp"
#include "event_system.hpp"
#include "request_state_change_event.hpp"
#include "state_ids.hpp"
#include "utils.hpp"


using std::string;


namespace going_in_circles {


//===========================================
// GameLogic::GameLogic
//===========================================
GameLogic::GameLogic(EventSystem& eventSystem, EntityManager& entityManager)
  : m_eventSystem(eventSystem),
    m_entityManager(entityManager) {

  DBG_PRINT("GameLogic::GameLogic\n");

}

//===========================================
// GameLogic::~GameLogic
//===========================================
GameLogic::~GameLogic() {

}


}
