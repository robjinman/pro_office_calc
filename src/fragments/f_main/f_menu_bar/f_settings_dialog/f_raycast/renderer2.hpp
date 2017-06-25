#ifndef __PROCALC_FRAGMENTS_F_RAYCAST_RENDERER_2_HPP__
#define __PROCALC_FRAGMENTS_F_RAYCAST_RENDERER_2_HPP__


#include <array>
#include <list>
#include "fragments/f_main/f_menu_bar/f_settings_dialog/f_raycast/scene.hpp"


class QImage;

struct Intersection {
  Intersection(Edge::kind_t kind)
    : kind(kind) {}

  Edge::kind_t kind;
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
    : Intersection(Edge::JOINING_EDGE) {}

  JoiningEdge* joiningEdge;
  Slice slice0;
  Slice slice1;
  const ConvexRegion* nearRegion;
  const ConvexRegion* farRegion;

  virtual ~JoiningEdgeX() {}
};

struct WallX : public Intersection {
  WallX()
    : Intersection(Edge::WALL) {}

  Wall* wall;
  Slice slice;

  virtual ~WallX() {}
};

struct CastResult {
  std::list<std::unique_ptr<Intersection>> intersections;
};

typedef std::array<double, 10000> tanMap_t;
typedef std::array<double, 10000> atanMap_t;

class Renderer2 {
  public:
    Renderer2();
    void renderScene(QImage& target, const Scene& scene);

  private:
    tanMap_t m_tanMap_rp;
    atanMap_t m_atanMap;
};

void findIntersections_r(const Camera& camera, const LineSegment& ray, const ConvexRegion& region,
  const Edge* exclude, CastResult& result);


#endif
