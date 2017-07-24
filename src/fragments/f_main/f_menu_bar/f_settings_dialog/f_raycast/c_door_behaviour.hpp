#ifndef __PROCALC_FRAGMENTS_F_RAYCAST_C_DOOR_BEHAVIOUR_HPP_
#define __PROCALC_FRAGMENTS_F_RAYCAST_C_DOOR_BEHAVIOUR_HPP_


#include "fragments/f_main/f_menu_bar/f_settings_dialog/f_raycast/behaviour_system.hpp"


class Region;
class CRegion;

class CDoorBehaviour : public CBehaviour {
  public:
    CDoorBehaviour(entityId_t entityId, Region& zone);

    virtual void update() override;
    virtual void handleEvent(const GameEvent& e) override;

  private:
    enum state_t {
      ST_OPENING,
      ST_CLOSING,
      ST_OPEN,
      ST_CLOSED
    };

    Region& m_zone;
    state_t m_state = ST_CLOSED;
    double m_y0;
    double m_y1;
};


#endif
