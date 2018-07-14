#ifndef __PROCALC_RAYCAST_RENDER_GRAPH_HPP__
#define __PROCALC_RAYCAST_RENDER_GRAPH_HPP__


#include <set>
#include "raycast/render_components.hpp"


struct RenderDefaults {
  std::string floorTexture = "default";
  std::string ceilingTexture = "default";
};

struct RenderGraph {
  RenderDefaults defaults;

  Size viewport;
  std::map<std::string, Texture> textures;

  Size viewport_px;

  double hWorldUnit_px;
  double vWorldUnit_px;

  pCRegion_t rootRegion;
  std::list<pCBoundary_t> boundaries;
  std::multiset<pCOverlay_t> overlays;
};


#endif
