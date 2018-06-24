#ifndef __PROCALC_FRAGMENTS_F_KERNEL_GAME_EVENTS_HPP__
#define __PROCALC_FRAGMENTS_F_KERNEL_GAME_EVENTS_HPP__


#include "raycast/component.hpp"
#include "raycast/game_event.hpp"


namespace doomsweeper {


struct EPlayerEnterCellInner : public GameEvent {
  explicit EPlayerEnterCellInner(entityId_t cellId)
    : GameEvent("player_enter_cell_inner"),
      cellId(cellId) {}

  entityId_t cellId;
};

struct ECellDoorOpened : public GameEvent {
  explicit ECellDoorOpened(entityId_t cellId)
    : GameEvent("cell_door_opened"),
      cellId(cellId) {}

  entityId_t cellId;
};


}


#endif
