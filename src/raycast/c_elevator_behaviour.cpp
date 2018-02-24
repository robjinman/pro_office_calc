#include <regex>
#include <algorithm>
#include "raycast/c_elevator_behaviour.hpp"
#include "raycast/c_switch_behaviour.hpp"
#include "raycast/entity_manager.hpp"
#include "raycast/spatial_components.hpp"
#include "event.hpp"
#include "exception.hpp"
#include "utils.hpp"


using std::vector;


//===========================================
// CElevatorBehaviour::CElevatorBehaviour
//===========================================
CElevatorBehaviour::CElevatorBehaviour(entityId_t entityId, EntityManager& entityManager,
  double frameRate, const vector<double>& levels)
  : CBehaviour(entityId),
    m_entityManager(entityManager),
    m_frameRate(frameRate),
    m_levels(levels) {

  if (m_levels.size() < 2) {
    EXCEPTION("Elevator must have at least 2 levels");
  }

  std::sort(m_levels.begin(), m_levels.end());
}

//===========================================
// CElevatorBehaviour::setSpeed
//===========================================
void CElevatorBehaviour::setSpeed(double speed) {
  m_speed = speed;
}

//===========================================
// CElevatorBehaviour::update
//===========================================
void CElevatorBehaviour::update() {
  CZone& zone = m_entityManager.getComponent<CZone>(entityId(), ComponentKind::C_SPATIAL);

  double y = zone.floorHeight;

  switch (m_state) {
    case ST_STOPPED:
      return;
    case ST_MOVING: {
      double targetY = m_levels[m_target];
      double dir = targetY > y ? 1.0 : -1.0;
      double dy = dir * m_speed / m_frameRate;

      zone.floorHeight += dy;

      if (fabs(zone.floorHeight + dy - targetY) < fabs(dy)) {
        m_state = ST_STOPPED;
        zone.floorHeight = targetY;

        m_entityManager.broadcastEvent(EElevatorStopped(entityId()));
      }

      break;
    }
  }
}

//===========================================
// CElevatorBehaviour::handleEvent
//===========================================
void CElevatorBehaviour::handleEvent(const GameEvent& e) {
  if (e.name == "switchActivate") {
    const ESwitchActivate& event = dynamic_cast<const ESwitchActivate&>(e);

    if (event.entityId == entityId()) {
      DBG_PRINT("Elevator activated\n");

      std::regex rx("level(\\d+(?:\\.\\d+)?)");
      std::smatch m;

      std::regex_match(event.message, m, rx);

      if (m.size() < 2) {
        EXCEPTION("Error parsing message '" << event.message << "' from switch");
      }

      m_target = std::stod(m.str(1));
      m_state = ST_MOVING;
    }
  }
}
