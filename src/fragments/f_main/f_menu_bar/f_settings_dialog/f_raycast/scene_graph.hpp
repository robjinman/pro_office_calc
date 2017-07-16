#ifndef __PROCALC_FRAGMENTS_F_RAYCAST_SCENE_GRAPH_HPP__
#define __PROCALC_FRAGMENTS_F_RAYCAST_SCENE_GRAPH_HPP__


#include "fragments/f_main/f_menu_bar/f_settings_dialog/f_raycast/scene_objects.hpp"
#include "fragments/f_main/f_menu_bar/f_settings_dialog/f_raycast/player.hpp"


struct SceneDefaults {
  double floorHeight = 0;
  double ceilingHeight = 100;
  std::string floorTexture = "cracked_mud";
  std::string ceilingTexture = "grey_stone";
};

struct SceneGraph {
  SceneDefaults defaults;

  Size viewport;
  std::map<std::string, Texture> textures;

  pRegion_t rootRegion;
  std::list<pEdge_t> edges;
  const Region* currentRegion;

  std::unique_ptr<Player> player;
};


#endif
