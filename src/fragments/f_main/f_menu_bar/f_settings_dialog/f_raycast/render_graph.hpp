#ifndef __PROCALC_FRAGMENTS_F_RAYCAST_RENDER_GRAPH_HPP__
#define __PROCALC_FRAGMENTS_F_RAYCAST_RENDER_GRAPH_HPP__


#include "fragments/f_main/f_menu_bar/f_settings_dialog/f_raycast/render_components.hpp"


struct RenderDefaults {
  double floorHeight = 0;
  double ceilingHeight = 100;
  std::string floorTexture = "cracked_mud";
  std::string ceilingTexture = "grey_stone";
};

struct RenderGraph {
  RenderDefaults defaults;

  Size viewport;
  std::map<std::string, Texture> textures;

  pCRegion_t rootRegion;
  std::list<pCEdge_t> edges;
};


#endif
