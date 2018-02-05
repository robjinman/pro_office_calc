#ifndef __PROCALC_RAYCAST_RENDER_GRAPH_HPP__
#define __PROCALC_RAYCAST_RENDER_GRAPH_HPP__


#include "raycast/render_components.hpp"


struct RenderDefaults {
  std::string floorTexture = "default";
  std::string ceilingTexture = "default";
};

struct RenderGraph {
  RenderDefaults defaults;

  Size viewport;
  std::map<std::string, Texture> textures;

  pCRegion_t rootRegion;
  std::list<pCBoundary_t> boundaries;
  std::list<pCOverlay_t> overlays;
};


#endif
