#include <regex>
#include <algorithm>
#include "raycast/c_elevator_behaviour.hpp"
#include "raycast/c_switch_behaviour.hpp"
#include "raycast/entity_manager.hpp"
#include "raycast/spatial_components.hpp"
#include "raycast/audio_service.hpp"
#include "event.hpp"
#include "exception.hpp"
#include "utils.hpp"


using std::vector;


//===========================================
// CElevatorBehaviour::CElevatorBehaviour
//===========================================
CElevatorBehaviour::CElevatorBehaviour(entityId_t entityId, EntityManager& entityManager,
  AudioService& audioService, double frameRate, const vector<double>& levels, int initLevelIdx)
  : CBehaviour(entityId),
    m_entityManager(entityManager),
    m_audioService(audioService),
    m_frameRate(frameRate),
    m_levels(levels) {

  if (m_levels.size() < 2) {
    EXCEPTION("Elevator must have at least 2 levels");
  }

  std::sort(m_levels.begin(), m_levels.end());

  if (initLevelIdx >= static_cast<int>(m_levels.size())) {
    EXCEPTION("Elevator's initial level index out of range");
  }

  m_target = initLevelIdx;

  CZone& zone = m_entityManager.getComponent<CZone>(this->entityId(), ComponentKind::C_SPATIAL);
  zone.floorHeight = m_levels[m_target];
}

//===========================================
// CElevatorBehaviour::playSound
//===========================================
void CElevatorBehaviour::playSound() const {
  CZone& zone = m_entityManager.getComponent<CZone>(entityId(), ComponentKind::C_SPATIAL);
  const Point& pos = zone.edges.front()->lseg.A;

  m_audioService.playSoundAtPos("elevator", pos, true);
}

//===========================================
// CElevatorBehaviour::stopSound
//===========================================
void CElevatorBehaviour::stopSound() const {
  m_audioService.stopSound("elevator");
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

        stopSound();
      }

      break;
    }
  }
}

//===========================================
// CElevatorBehaviour::move
//===========================================
void CElevatorBehaviour::move(int level) {
  m_target = level;
  m_state = ST_MOVING;

  playSound();
}

//===========================================
// CElevatorBehaviour::handleTargetedEvent
//===========================================
void CElevatorBehaviour::handleTargetedEvent(const GameEvent& e) {
  if (e.name == "switch_activated") {
    const ESwitchActivate& event = dynamic_cast<const ESwitchActivate&>(e);

    DBG_PRINT("Elevator activated\n");

    std::regex rx("level(\\d+(?:\\.\\d+)?)");
    std::smatch m;

    std::regex_match(event.message, m, rx);

    if (m.size() < 2) {
      EXCEPTION("Error parsing message '" << event.message << "' from switch");
    }

    move(std::stod(m.str(1)));
  }
  else if (e.name == "player_activate_entity" && this->isPlayerActivated) {
    if (m_state == ST_STOPPED) {
      m_target = (m_target + 1) % m_levels.size();
    }

    m_state = ST_MOVING;
    playSound();
  }
}
