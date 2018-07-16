#ifndef __PROCALC_RAYCAST_C_SOUND_SOURCE_BEHAVIOUR_HPP_
#define __PROCALC_RAYCAST_C_SOUND_SOURCE_BEHAVIOUR_HPP_


#include "raycast/behaviour_system.hpp"
#include "raycast/geometry.hpp"


class AudioService;

class CSoundSourceBehaviour : public CBehaviour {
  public:
    CSoundSourceBehaviour(entityId_t entityId, const std::string& name, const Point& pos,
      double radius, AudioService& audioService);

    std::string name;
    Point pos;
    double radius;

    void update() override {}
    void handleBroadcastedEvent(const GameEvent& event) override;
    void handleTargetedEvent(const GameEvent& event) override;

    void setDisabled(bool disabled);

    ~CSoundSourceBehaviour() override;

  private:
    AudioService& m_audioService;
    bool m_disabled = false;
    int m_soundId = -1;
};


#endif
