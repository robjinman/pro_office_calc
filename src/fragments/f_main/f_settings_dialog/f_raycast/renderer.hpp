#ifndef __PROCALC_FRAGMENTS_F_RAYCAST_RENDERER_HPP__
#define __PROCALC_FRAGMENTS_F_RAYCAST_RENDERER_HPP__


#include <array>
#include <list>
#include <set>
#include <QPainter>
#include "fragments/f_main/f_settings_dialog/f_raycast/spatial_components.hpp"
#include "fragments/f_main/f_settings_dialog/f_raycast/render_graph.hpp"


class QImage;
class SceneGraph;

typedef std::array<double, 10000> tanMap_t;
typedef std::array<double, 10000> atanMap_t;

class EntityManager;
class Player;
class SpatialSystem;
class SpriteX;

class Renderer {
  public:
    Renderer(EntityManager& entityManager, QImage& target);

    void renderScene(const RenderGraph& rg, const Player& player);

  private:
    EntityManager& m_entityManager;

    QImage& m_target;

    tanMap_t m_tanMap_rp;
    atanMap_t m_atanMap;
};


#endif
