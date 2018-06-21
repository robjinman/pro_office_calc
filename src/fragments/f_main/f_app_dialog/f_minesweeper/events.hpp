#ifndef __PROCALC_FRAGMENTS_F_MINESWEEPER_EVENTS_HPP__
#define __PROCALC_FRAGMENTS_F_MINESWEEPER_EVENTS_HPP__


#include <set>
#include "event.hpp"


namespace millennium_bug {


struct Coord {
  int row;
  int col;
};

inline bool operator==(const Coord& lhs, const Coord& rhs) {
  return lhs.row == rhs.row && lhs.col == rhs.col;
}

inline bool operator<(const Coord& lhs, const Coord& rhs) {
  if (lhs.row == rhs.row) {
    return lhs.col < rhs.col;
  }
  else {
    return lhs.row < rhs.row;
  }
}

struct MinesweeperSetupEvent : public Event {
  MinesweeperSetupEvent(const std::set<Coord>& coords)
    : Event("millenniumBug/minesweeperSetupComplete"),
      mineCoords(coords) {}

    std::set<Coord> mineCoords;
};

struct CellEnteredEvent : public Event {
  CellEnteredEvent(const Coord& coords)
    : Event("millenniumBug/cellEntered"),
      coords(coords) {}

    Coord coords;
};


}


#endif
