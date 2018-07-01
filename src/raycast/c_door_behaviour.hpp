#ifndef __PROCALC_RAYCAST_C_DOOR_BEHAVIOUR_HPP_
#define __PROCALC_RAYCAST_C_DOOR_BEHAVIOUR_HPP_


#include "raycast/behaviour_system.hpp"
#include "raycast/timing.hpp"


struct EDoorOpenStart : public GameEvent {
  explicit EDoorOpenStart(entityId_t entityId)
    : GameEvent("door_open_start"),
      entityId(entityId) {}

  entityId_t entityId;
};

struct EDoorOpenFinish : public GameEvent {
  explicit EDoorOpenFinish(entityId_t entityId)
    : GameEvent("door_open_finish"),
      entityId(entityId) {}

  entityId_t entityId;
};

struct EDoorCloseStart : public GameEvent {
  explicit EDoorCloseStart(entityId_t entityId)
    : GameEvent("door_close_start"),
      entityId(entityId) {}

  entityId_t entityId;
};

struct EDoorCloseFinish : public GameEvent {
  explicit EDoorCloseFinish(entityId_t entityId)
    : GameEvent("door_close_finish"),
      entityId(entityId) {}

  entityId_t entityId;
};

class EntityManager;
class AudioService;
class TimeService;

class CDoorBehaviour : public CBehaviour {
  public:
    CDoorBehaviour(entityId_t entityId, EntityManager& entityManager, TimeService& timeService,
      AudioService& audioService);

    virtual void update() override;
    virtual void handleBroadcastedEvent(const GameEvent& e) override;
    virtual void handleTargetedEvent(const GameEvent& e) override;

    double speed = 100.0;
    bool isPlayerActivated = true;
    bool closeAutomatically = true;
    std::string openOnEvent = "";

    void setPauseTime(double t);

  private:
    enum state_t {
      ST_OPENING,
      ST_CLOSING,
      ST_OPEN,
      ST_CLOSED
    };

    EntityManager& m_entityManager;
    TimeService& m_timeService;
    AudioService& m_audioService;
    state_t m_state = ST_CLOSED;
    double m_y0;
    double m_y1;
    Debouncer m_timer;

    void playSound() const;
    void stopSound() const;
};


#endif
