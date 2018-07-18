#include "raycast/c_door_behaviour.hpp"
#include "raycast/spatial_system.hpp"
#include "raycast/render_system.hpp"
#include "raycast/focus_system.hpp"
#include "raycast/entity_manager.hpp"
#include "raycast/audio_service.hpp"
#include "raycast/time_service.hpp"
#include "event.hpp"


using std::string;


//===========================================
// CDoorBehaviour::CDoorBehaviour
//===========================================
CDoorBehaviour::CDoorBehaviour(entityId_t entityId, EntityManager& entityManager,
  TimeService& timeService, AudioService& audioService)
  : CBehaviour(entityId),
    m_entityManager(entityManager),
    m_timeService(timeService),
    m_audioService(audioService),
    m_timer(5.0) {

  CZone& zone = entityManager.getComponent<CZone>(entityId, ComponentKind::C_SPATIAL);

  m_y0 = zone.floorHeight;
  m_y1 = zone.ceilingHeight;

  zone.ceilingHeight = zone.floorHeight + 0.1;
}

//===========================================
// CDoorBehaviour::setPauseTime
//===========================================
void CDoorBehaviour::setPauseTime(double t) {
  m_timer = Debouncer{t};
}

//===========================================
// CDoorBehaviour::playSound
//===========================================
void CDoorBehaviour::playSound() {
  CZone& zone = m_entityManager.getComponent<CZone>(entityId(), ComponentKind::C_SPATIAL);
  const Point& pos = zone.edges.front()->lseg.A;

  if (!m_audioService.soundIsPlaying("door", m_soundId)) {
    m_soundId = m_audioService.playSoundAtPos("door", pos, true);
  }
}

//===========================================
// CDoorBehaviour::stopSound
//===========================================
void CDoorBehaviour::stopSound() const {
  m_audioService.stopSound("door", m_soundId);
}

//===========================================
// CDoorBehaviour::update
//===========================================
void CDoorBehaviour::update() {
  SpatialSystem& spatialSystem = m_entityManager.system<SpatialSystem>(ComponentKind::C_SPATIAL);
  Player& player = *spatialSystem.sg.player;
  CZone& zone = m_entityManager.getComponent<CZone>(entityId(), ComponentKind::C_SPATIAL);

  double dy = this->speed / m_timeService.frameRate;

  switch (m_state) {
    case ST_CLOSED:
      return;
    case ST_OPEN:
      if (closeAutomatically && m_timer.ready()) {
        m_state = ST_CLOSING;
        playSound();

        m_entityManager.fireEvent(EDoorCloseStart{entityId()}, { entityId() });
        m_entityManager.broadcastEvent(EDoorCloseStart{entityId()});
      }
      return;
    case ST_OPENING:
      zone.ceilingHeight += dy;

      if (zone.ceilingHeight + dy >= m_y1) {
        m_state = ST_OPEN;
        stopSound();

        m_entityManager.fireEvent(EDoorOpenFinish{entityId()}, { entityId() });
        m_entityManager.broadcastEvent(EDoorOpenFinish{entityId()});
      }
      else {
        m_timer.reset();
      }

      break;
    case ST_CLOSING:
      zone.ceilingHeight -= dy;

      if (player.region() == entityId()) {
        if (zone.ceilingHeight - dy <= player.headHeight()) {
          m_state = ST_OPENING;
          m_entityManager.fireEvent(EDoorOpenStart{entityId()}, { entityId() });
          m_entityManager.broadcastEvent(EDoorOpenStart{entityId()});
        }
      }
      else if (zone.ceilingHeight - dy <= m_y0) {
        m_state = ST_CLOSED;
        stopSound();
        m_entityManager.fireEvent(EDoorCloseFinish{entityId()}, { entityId() });
        m_entityManager.broadcastEvent(EDoorCloseFinish{entityId()});
      }

      break;
  }
}

//===========================================
// CDoorBehaviour::handleBroadcastedEvent
//===========================================
void CDoorBehaviour::handleBroadcastedEvent(const GameEvent& e) {
  if (e.name == openOnEvent) {
    if (m_state != ST_OPEN) {
      m_state = ST_OPENING;
      playSound();
    }
  }
}

//===========================================
// CDoorBehaviour::handleTargetedEvent
//===========================================
void CDoorBehaviour::handleTargetedEvent(const GameEvent& e) {
  const std::set<std::string> EVENT_NAMES{
    "activate_entity",
    "player_activate_entity",
    "switch_activated"
  };

  if (!isPlayerActivated && e.name == "player_activate_entity") {
    if (m_state != ST_OPEN) {
      auto& focusSystem = m_entityManager.system<FocusSystem>(ComponentKind::C_FOCUS);

      if (focusSystem.hasComponent(entityId())) {
        focusSystem.showCaption(entityId());
      }
    }

    return;
  }

  if (EVENT_NAMES.count(e.name)) {
    switch (m_state) {
      case ST_CLOSED:
        m_state = ST_OPENING;
        playSound();
        m_entityManager.fireEvent(EDoorOpenStart{entityId()}, { entityId() });
        m_entityManager.broadcastEvent(EDoorOpenStart{entityId()});
        break;
      case ST_OPEN:
        m_state = ST_CLOSING;
        playSound();
        m_entityManager.fireEvent(EDoorCloseStart{entityId()}, { entityId() });
        m_entityManager.broadcastEvent(EDoorCloseStart{entityId()});
        break;
      case ST_OPENING:
        m_state = ST_CLOSING;
        m_entityManager.fireEvent(EDoorCloseStart{entityId()}, { entityId() });
        m_entityManager.broadcastEvent(EDoorCloseStart{entityId()});
        break;
      case ST_CLOSING:
        m_state = ST_OPENING;
        m_entityManager.fireEvent(EDoorOpenStart{entityId()}, { entityId() });
        m_entityManager.broadcastEvent(EDoorOpenStart{entityId()});
        break;
    }
  }
}
