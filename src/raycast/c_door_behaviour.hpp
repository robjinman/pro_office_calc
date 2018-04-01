#ifndef __PROCALC_RAYCAST_C_DOOR_BEHAVIOUR_HPP_
#define __PROCALC_RAYCAST_C_DOOR_BEHAVIOUR_HPP_


#include "raycast/behaviour_system.hpp"
#include "raycast/timing.hpp"


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

    bool isPlayerActivated = true;
    bool closeAutomatically = true;
    std::string openOnEvent = "";

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

    entityId_t m_captionBgId = -1;
    entityId_t m_captionTextId = -1;
    long m_captionTimeoutId = -1;

    void playSound() const;
    void stopSound() const;

    void showCaption();
    void deleteCaption();
};


#endif
