#ifndef __PROCALC_FRAGMENTS_F_RAYCAST_C_SWITCH_BEHAVIOUR_HPP_
#define __PROCALC_FRAGMENTS_F_RAYCAST_C_SWITCH_BEHAVIOUR_HPP_


#include "fragments/f_main/f_settings_dialog/f_raycast/behaviour_system.hpp"
#include "fragments/f_main/f_settings_dialog/f_raycast/timing.hpp"


class EntityManager;
class CWallDecal;

class CSwitchBehaviour : public CBehaviour {
  public:
    CSwitchBehaviour(entityId_t entityId, EntityManager& entityManager, entityId_t target,
      bool toggleable, double toggleDelay);

    virtual void update() override;
    virtual void handleEvent(const GameEvent& e) override;

    virtual ~CSwitchBehaviour() override {}

  private:
    enum state_t {
      ST_ON,
      ST_OFF
    };

    EntityManager& m_entityManager;
    entityId_t m_target;
    state_t m_state = ST_OFF;
    bool m_toggleable;
    double m_toggleDelay;
    Debouncer m_timer;

    CWallDecal* getDecal() const;
};


#endif
