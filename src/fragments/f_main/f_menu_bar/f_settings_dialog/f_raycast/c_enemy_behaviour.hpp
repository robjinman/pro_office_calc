#ifndef __PROCALC_FRAGMENTS_F_RAYCAST_C_ENEMY_BEHAVIOUR_HPP_
#define __PROCALC_FRAGMENTS_F_RAYCAST_C_ENEMY_BEHAVIOUR_HPP_


#include "fragments/f_main/f_menu_bar/f_settings_dialog/f_raycast/behaviour_system.hpp"


class EntityManager;

class CEnemyBehaviour : public CBehaviour {
  public:
    CEnemyBehaviour(entityId_t entityId, EntityManager& entityManager);

    virtual void update() override;
    virtual void handleEvent(const GameEvent& e) override;

  private:
    EntityManager& m_entityManager;
};


#endif
