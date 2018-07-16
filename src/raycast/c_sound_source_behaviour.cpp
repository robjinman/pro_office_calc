#include "raycast/c_sound_source_behaviour.hpp"
#include "audio_service.hpp"
#include "spatial_system.hpp"


//===========================================
// CSoundSourceBehaviour::CSoundSourceBehaviour
//===========================================
CSoundSourceBehaviour::CSoundSourceBehaviour(entityId_t entityId, const std::string& name,
  const Point& pos, double radius, AudioService& audioService)
  : CBehaviour(entityId),
    name(name),
    pos(pos),
    radius(radius),
    m_audioService(audioService) {}

//===========================================
// CSoundSourceBehaviour::handleBroadcastedEvent
//===========================================
void CSoundSourceBehaviour::handleBroadcastedEvent(const GameEvent& e_) {
  if (e_.name == "player_move" && !m_disabled) {
    auto& e = dynamic_cast<const EPlayerMove&>(e_);

    if (distance(pos, e.player.pos()) <= this->radius) {
      if (!m_audioService.soundIsPlaying(this->name, m_soundId)) {
        DBG_PRINT("Playing sound " << this->name << " from source " << entityId() << "\n");
        m_soundId = m_audioService.playSoundAtPos(this->name, this->pos, true);
      }
    }
    else {
      if (m_audioService.soundIsPlaying(this->name, m_soundId)) {
        DBG_PRINT("Stopping sound " << this->name << " from source " << entityId() << "\n");
        m_audioService.stopSound(this->name, m_soundId);
      }
    }
  }
}

//===========================================
// CSoundSourceBehaviour::handleTargetedEvent
//===========================================
void CSoundSourceBehaviour::handleTargetedEvent(const GameEvent& e_) {
  if (e_.name == "disable_sound_source") {
    setDisabled(true);
  }
  else if (e_.name == "enable_sound_source") {
    setDisabled(false);
  }
}

//===========================================
// CSoundSourceBehaviour::setDisabled
//===========================================
void CSoundSourceBehaviour::setDisabled(bool disabled) {
  m_disabled = disabled;

  if (m_disabled) {
    m_audioService.stopSound(this->name, m_soundId);
  }
}

//===========================================
// CSoundSourceBehaviour::~CSoundSourceBehaviour
//===========================================
CSoundSourceBehaviour::~CSoundSourceBehaviour() {}
