#ifndef __PROCALC_FRAGMENTS_F_RAYCAST_SCENE_GRAPH_HPP__
#define __PROCALC_FRAGMENTS_F_RAYCAST_SCENE_GRAPH_HPP__


#include "fragments/f_main/f_settings_dialog/f_raycast/spatial_components.hpp"
#include "fragments/f_main/f_settings_dialog/f_raycast/player.hpp"


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
