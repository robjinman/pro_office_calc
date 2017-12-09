#ifndef __PROCALC_RAYCAST_SCENE_GRAPH_HPP__
#define __PROCALC_RAYCAST_SCENE_GRAPH_HPP__


#include "raycast/spatial_components.hpp"
#include "raycast/player.hpp"


struct SceneDefaults {
  double floorHeight = 0;
  double ceilingHeight = 100;
};

struct SceneGraph {
  SceneDefaults defaults;

  pCZone_t rootZone;
  std::list<pCEdge_t> edges;

  std::unique_ptr<Player> player;
};


#endif
