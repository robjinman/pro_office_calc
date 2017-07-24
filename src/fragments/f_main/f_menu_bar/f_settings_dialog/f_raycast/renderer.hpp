#ifndef __PROCALC_FRAGMENTS_F_RAYCAST_RENDERER_HPP__
#define __PROCALC_FRAGMENTS_F_RAYCAST_RENDERER_HPP__


#include <array>
#include <list>
#include <set>
#include "fragments/f_main/f_menu_bar/f_settings_dialog/f_raycast/system.hpp"
#include "fragments/f_main/f_menu_bar/f_settings_dialog/f_raycast/scene_objects.hpp"
#include "fragments/f_main/f_menu_bar/f_settings_dialog/f_raycast/render_graph.hpp"


class QImage;
class SceneGraph;

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

  CJoiningEdge* joiningEdge;
  Slice slice0;
  Slice slice1;
  const CRegion* nearRegion;
  const CRegion* farRegion;

  virtual ~JoiningEdgeX() {}
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

class Renderer : public System {
  public:
    Renderer(EntityManager& entityManager);

    void connectRegions();

    virtual void update() override;
    virtual void handleEvent(const GameEvent& event) override;

    virtual void addComponent(pComponent_t component) override;
    virtual Component& getComponent(entityId_t entityId) const override;
    virtual void removeEntity(entityId_t id) override;

    void renderScene(QImage& target, const Player& player);

    RenderGraph rg;

  private:
    EntityManager& m_entityManager;

    tanMap_t m_tanMap_rp;
    atanMap_t m_atanMap;

    std::map<entityId_t, CRender*> m_components;
    std::map<entityId_t, std::set<entityId_t>> m_entityChildren;

    bool isRoot(const CRender& c) const;
    void removeEntity_r(entityId_t id);

    inline CRegion& region(entityId_t id) const {
      return dynamic_cast<CRegion&>(*m_components.at(id));
    }
};

void findIntersections_r(const Camera& camera, const LineSegment& ray, const Region& region,
  CastResult& result, std::set<const Region*>& visitedRegions,
  std::set<entityId_t>& visitedJoiningEdges);


#endif
