#include <gtest/gtest.h>
#include <raycast/spatial_system.hpp>


using std::list;
using std::set;


class SpatialSystemTest : public testing::Test {
  public:
    virtual void SetUp() override {}

    virtual void TearDown() override {}
};

TEST_F(SpatialSystemTest, singleZone) {
  Point pos(50, 30);
  Vec2f dir(0, 1);
  Matrix m;

  CZone* zone = new CZone(Component::getNextId(), -1);
  CHardEdge* edge0 = new CHardEdge(Component::getNextId(), zone->entityId());
  edge0->zone = zone;
  edge0->lseg = LineSegment(Point(0, 0), Point(100, 0));
  CHardEdge* edge1 = new CHardEdge(Component::getNextId(), zone->entityId());
  edge1->zone = zone;
  edge1->lseg = LineSegment(Point(100, 0), Point(100, 100));
  CHardEdge* edge2 = new CHardEdge(Component::getNextId(), zone->entityId());
  edge2->zone = zone;
  edge2->lseg = LineSegment(Point(100, 100), Point(0, 100));
  CHardEdge* edge3 = new CHardEdge(Component::getNextId(), zone->entityId());
  edge3->zone = zone;
  edge3->lseg = LineSegment(Point(0, 100), Point(0, 0));

  zone->edges.push_back(edge0);
  zone->edges.push_back(edge1);
  zone->edges.push_back(edge2);
  zone->edges.push_back(edge3);

  list<pIntersection_t> intersections;
  set<const CZone*> visitedZones;
  set<entityId_t> visitedJoins;
  findIntersections_r(pos, dir, m, *zone, intersections, visitedZones, visitedJoins);

  ASSERT_EQ(1, intersections.size());
}

TEST_F(SpatialSystemTest, nestedZone) {
  CZone* zone0 = new CZone(Component::getNextId(), -1);

  CZone* zone1 = new CZone(Component::getNextId(), zone0->entityId());
  zone1->parent = zone0;

  CSoftEdge* se0 = new CSoftEdge(Component::getNextId(), zone1->entityId(), Component::getNextId());
  se0->lseg = LineSegment(Point(20, 20), Point(80, 20));
  CSoftEdge* se1 = new CSoftEdge(Component::getNextId(), zone1->entityId(), Component::getNextId());
  se1->lseg = LineSegment(Point(80, 20), Point(80, 80));
  CSoftEdge* se2 = new CSoftEdge(Component::getNextId(), zone1->entityId(), Component::getNextId());
  se2->lseg = LineSegment(Point(80, 80), Point(20, 80));
  CSoftEdge* se3 = new CSoftEdge(Component::getNextId(), zone1->entityId(), Component::getNextId());
  se3->lseg = LineSegment(Point(20, 80), Point(20, 20));

  zone1->edges.push_back(se0);
  zone1->edges.push_back(se1);
  zone1->edges.push_back(se2);
  zone1->edges.push_back(se3);

  zone0->children.push_back(pCZone_t(zone1));

  CHardEdge* he0 = new CHardEdge(Component::getNextId(), zone0->entityId());
  he0->zone = zone0;
  he0->lseg = LineSegment(Point(0, 0), Point(100, 0));
  CHardEdge* he1 = new CHardEdge(Component::getNextId(), zone0->entityId());
  he1->zone = zone0;
  he1->lseg = LineSegment(Point(100, 0), Point(100, 100));
  CHardEdge* he2 = new CHardEdge(Component::getNextId(), zone0->entityId());
  he2->zone = zone0;
  he2->lseg = LineSegment(Point(100, 100), Point(0, 100));
  CHardEdge* he3 = new CHardEdge(Component::getNextId(), zone0->entityId());
  he3->zone = zone0;
  he3->lseg = LineSegment(Point(0, 100), Point(0, 0));

  zone0->edges.push_back(he0);
  zone0->edges.push_back(he1);
  zone0->edges.push_back(he2);
  zone0->edges.push_back(he3);

  connectSubzones(*zone0);

  list<pIntersection_t> intersections;
  set<const CZone*> visitedZones;
  set<entityId_t> visitedJoins;
  findIntersections_r(Point(50, 10), Vec2f(0, 1), Matrix(), *zone0, intersections, visitedZones,
    visitedJoins);

  ASSERT_EQ(3, intersections.size());

  intersections.clear();
  visitedZones.clear();
  visitedJoins.clear();
  findIntersections_r(Point(50, 30), Vec2f(0, 1), Matrix(), *zone0, intersections, visitedZones,
    visitedJoins);

  ASSERT_EQ(2, intersections.size());
}

TEST_F(SpatialSystemTest, zoneWithVrect) {
  CZone* zone = new CZone(Component::getNextId(), -1);
  CHardEdge* edge0 = new CHardEdge(Component::getNextId(), zone->entityId());
  edge0->zone = zone;
  edge0->lseg = LineSegment(Point(0, 0), Point(100, 0));
  CHardEdge* edge1 = new CHardEdge(Component::getNextId(), zone->entityId());
  edge1->zone = zone;
  edge1->lseg = LineSegment(Point(100, 0), Point(100, 100));
  CHardEdge* edge2 = new CHardEdge(Component::getNextId(), zone->entityId());
  edge2->zone = zone;
  edge2->lseg = LineSegment(Point(100, 100), Point(0, 100));
  CHardEdge* edge3 = new CHardEdge(Component::getNextId(), zone->entityId());
  edge3->zone = zone;
  edge3->lseg = LineSegment(Point(0, 100), Point(0, 0));

  zone->edges.push_back(edge0);
  zone->edges.push_back(edge1);
  zone->edges.push_back(edge2);
  zone->edges.push_back(edge3);

  CVRect* vRect = new CVRect(Component::getNextId(), zone->entityId(), Size(20, 20));
  vRect->zone = zone;
  vRect->pos = Point(50, 50);

  zone->vRects.push_back(pCVRect_t(vRect));

  Point pos(50, 30);
  Vec2f ray(0, 1);
  Matrix m;

  list<pIntersection_t> intersections;
  set<const CZone*> visitedZones;
  set<entityId_t> visitedJoins;
  findIntersections_r(pos, ray, m, *zone, intersections, visitedZones, visitedJoins);

  //ASSERT_EQ(2, intersections.size());
}
