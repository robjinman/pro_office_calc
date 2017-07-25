#ifndef __PROCALC_FRAGMENTS_F_RAYCAST_RENDERER_HPP__
#define __PROCALC_FRAGMENTS_F_RAYCAST_RENDERER_HPP__


#include <array>
#include <list>
#include <set>
#include "fragments/f_main/f_menu_bar/f_settings_dialog/f_raycast/spatial_components.hpp"
#include "fragments/f_main/f_menu_bar/f_settings_dialog/f_raycast/render_graph.hpp"


class QImage;
class SceneGraph;

enum class IntersectionKind {
  JOIN,
  WALL,
  SPRITE
};

struct Intersection {
  Intersection(IntersectionKind kind)
    : kind(kind) {}

  IntersectionKind kind;
  Point point_cam;
  Point point_world;
  double distanceFromCamera;
  double distanceAlongTarget;

  virtual ~Intersection() {}
};

typedef std::unique_ptr<Intersection> pIntersection_t;

struct Slice {
  double sliceBottom_wd;
  double sliceTop_wd;
  double projSliceBottom_wd;
  double projSliceTop_wd;
  double viewportBottom_wd;
  double viewportTop_wd;
};

struct JoinX : public Intersection {
  JoinX()
    : Intersection(IntersectionKind::JOIN) {}

  CJoin* join;
  Slice slice0;
  Slice slice1;
  const CRegion* nearRegion;
  const CRegion* farRegion;

  virtual ~JoinX() {}
};

struct WallX : public Intersection {
  WallX()
    : Intersection(IntersectionKind::WALL) {}

  CWall* wall;
  Slice slice;

  virtual ~WallX() {}
};

struct SpriteX : public Intersection {
  SpriteX()
    : Intersection(IntersectionKind::SPRITE) {}

  CSprite* sprite;
  Slice slice;

  virtual ~SpriteX() {}
};

struct CastResult {
  std::list<pIntersection_t> intersections;
};

typedef std::array<double, 10000> tanMap_t;
typedef std::array<double, 10000> atanMap_t;

class EntityManager;
class Player;

class Renderer {
  public:
    Renderer(EntityManager& entityManager);

    void renderScene(QImage& target, const RenderGraph& rg, const Player& player,
      const CRegion& currentRegion);

  private:
    EntityManager& m_entityManager;

    tanMap_t m_tanMap_rp;
    atanMap_t m_atanMap;
};

void findIntersections_r(const Camera& camera, const LineSegment& ray, const CRegion& region,
  CastResult& result, std::set<const CRegion*>& visitedRegions, std::set<entityId_t>& visitedJoins);


#endif
