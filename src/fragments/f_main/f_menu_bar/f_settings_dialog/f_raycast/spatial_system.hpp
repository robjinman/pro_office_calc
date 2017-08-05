#ifndef __PROCALC_FRAGMENTS_F_RAYCAST_SPATIAL_SYSTEM_HPP__
#define __PROCALC_FRAGMENTS_F_RAYCAST_SPATIAL_SYSTEM_HPP__


#include <string>
#include <list>
#include <map>
#include <set>
#include <memory>
#include <QImage>
#include "fragments/f_main/f_menu_bar/f_settings_dialog/f_raycast/scene_graph.hpp"
#include "fragments/f_main/f_menu_bar/f_settings_dialog/f_raycast/system.hpp"


namespace tinyxml2 { class XMLElement; }
namespace parser { class Object; }


struct Tween {
  std::function<bool()> tick;
  std::function<void()> finish;
};

enum class IntersectionKind {
  SOFT_EDGE,
  HARD_EDGE,
  V_RECT
};

struct Intersection {
  Intersection(IntersectionKind kind)
    : kind(kind) {}

  IntersectionKind kind;
  entityId_t entityId;
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

struct SoftEdgeX : public Intersection {
  SoftEdgeX()
    : Intersection(IntersectionKind::SOFT_EDGE) {}

  CSoftEdge* softEdge;
  Slice slice0;
  Slice slice1;
  const CZone* nearZone;
  const CZone* farZone;

  virtual ~SoftEdgeX() override {}
};

struct HardEdgeX : public Intersection {
  HardEdgeX()
    : Intersection(IntersectionKind::HARD_EDGE) {}

  CHardEdge* hardEdge;
  Slice slice;

  virtual ~HardEdgeX() override {}
};

struct VRectX : public Intersection {
  VRectX()
    : Intersection(IntersectionKind::V_RECT) {}

  CVRect* vRect;
  Slice slice;

  virtual ~VRectX() override {}
};

class EntityManager;

class SpatialSystem : public System {
  public:
    SpatialSystem(EntityManager& entityManager, double frameRate);

    SceneGraph sg;

    // TODO: Move this out of this class
    void loadMap(const std::string& mapFilePath);

    void connectZones();

    virtual void update() override;
    virtual void handleEvent(const GameEvent& event) override;

    virtual void addComponent(pComponent_t component) override;
    virtual bool hasComponent(entityId_t entityId) const override;
    virtual Component& getComponent(entityId_t entityId) const override;
    virtual void removeEntity(entityId_t id) override;

    std::set<entityId_t> entitiesInRadius(const Point& pos, double radius) const;
    std::list<pIntersection_t> entitiesAlongRay(double camSpaceAngle) const;

    void vRotateCamera(double da);
    void hRotateCamera(double da);
    void translateCamera(const Vec2f& dir);
    void jump();
    void addTween(const Tween& tween, const char* name = nullptr);

    virtual ~SpatialSystem() override;

  private:
    EntityManager& m_entityManager;

    double m_frameRate;
    std::map<std::string, Tween> m_tweens;

    std::map<entityId_t, CSpatial*> m_components;
    std::map<entityId_t, std::set<entityId_t>> m_entityChildren;

    Vec2i m_playerCell;

    bool isRoot(const CSpatial& c) const;
    void removeEntity_r(entityId_t id);

    inline CZone& getCurrentZone() const {
      return dynamic_cast<CZone&>(*m_components.at(sg.player->currentRegion));
    }

    void buoyancy();
    void gravity();
};


#endif
