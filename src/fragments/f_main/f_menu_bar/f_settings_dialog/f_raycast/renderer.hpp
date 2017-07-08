#ifndef __PROCALC_FRAGMENTS_F_RAYCAST_RENDERER_2_HPP__
#define __PROCALC_FRAGMENTS_F_RAYCAST_RENDERER_2_HPP__


#include <array>
#include <list>
#include <set>
#include "fragments/f_main/f_menu_bar/f_settings_dialog/f_raycast/scene.hpp"


class QImage;

enum class IntersectionKind {
  JOINING_EDGE,
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

struct JoiningEdgeX : public Intersection {
  JoiningEdgeX()
    : Intersection(IntersectionKind::JOINING_EDGE) {}

  JoiningEdge* joiningEdge;
  Slice slice0;
  Slice slice1;
  const Region* nearRegion;
  const Region* farRegion;

  virtual ~JoiningEdgeX() {}
};

struct WallX : public Intersection {
  WallX()
    : Intersection(IntersectionKind::WALL) {}

  Wall* wall;
  Slice slice;

  virtual ~WallX() {}
};

struct SpriteX : public Intersection {
  SpriteX()
    : Intersection(IntersectionKind::SPRITE) {}

  Sprite* sprite;
  Slice slice;

  virtual ~SpriteX() {}
};

struct CastResult {
  std::list<pIntersection_t> intersections;
};

typedef std::array<double, 10000> tanMap_t;
typedef std::array<double, 10000> atanMap_t;

class Renderer {
  public:
    Renderer();
    void renderScene(QImage& target, const Scene& scene);

  private:
    tanMap_t m_tanMap_rp;
    atanMap_t m_atanMap;
};

void findIntersections_r(const Camera& camera, const LineSegment& ray, const Region& region,
  CastResult& result, std::set<const Region*>& visitedRegions,
  std::set<const JoiningEdge*>& visitedJoiningEdges);


#endif
