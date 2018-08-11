#ifndef __PROCALC_RAYCAST_C_ELEVATOR_BEHAVIOUR_HPP_
#define __PROCALC_RAYCAST_C_ELEVATOR_BEHAVIOUR_HPP_


#include <vector>
#include "raycast/behaviour_system.hpp"


struct EElevatorStopped : public GameEvent {
  explicit EElevatorStopped(entityId_t entityId)
    : GameEvent("elevator_stopped"),
      entityId(entityId) {}

  entityId_t entityId;

  virtual ~EElevatorStopped() override {}
};

class EntityManager;
class AudioService;

class CElevatorBehaviour : public CBehaviour {
  public:
    CElevatorBehaviour(entityId_t entityId, EntityManager& entityManager,
      AudioService& audioService, double frameRate, const std::vector<double>& levels,
      int initLevelIdx);

    bool isPlayerActivated = false;

    virtual void update() override;
    virtual void handleBroadcastedEvent(const GameEvent&) override {}
    virtual void handleTargetedEvent(const GameEvent&) override;

    void move(int level);
    void setSpeed(double speed);

  private:
    void playSound();
    void stopSound() const;

    enum state_t {
      ST_STOPPED,
      ST_MOVING
    };

    EntityManager& m_entityManager;
    AudioService& m_audioService;
    double m_frameRate;
    state_t m_state = ST_STOPPED;
    int m_target = 0;
    double m_speed = 60.0;
    std::vector<double> m_levels;
    int m_soundId = -1;
};


#endif
