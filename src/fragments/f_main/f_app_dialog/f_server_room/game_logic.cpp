#include "fragments/f_main/f_app_dialog/f_server_room/game_logic.hpp"
#include "raycast/entity_manager.hpp"
#include "event_system.hpp"
#include "utils.hpp"


using std::string;


namespace youve_got_mail {


//===========================================
// GameLogic::GameLogic
//===========================================
GameLogic::GameLogic(EventSystem& eventSystem, EntityManager& entityManager)
  : m_eventSystem(eventSystem),
    m_entityManager(entityManager) {

  DBG_PRINT("GameLogic::GameLogic\n");

  m_eventSystem.listen("youveGotMail/divByZero", [this](const Event& event) {
    onDivByZero(event);
  }, m_eventIdx);
}

//===========================================
// GameLogic::onDivByZero
//===========================================
void GameLogic::onDivByZero(const Event& event) {
  m_entityManager.broadcastEvent(GameEvent("div_by_zero"));
}

//===========================================
// GameLogic::~GameLogic
//===========================================
GameLogic::~GameLogic() {
  m_eventSystem.forget(m_eventIdx);
}


}
