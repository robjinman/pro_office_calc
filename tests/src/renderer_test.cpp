#include <gtest/gtest.h>
#include <list>
#include <fragments/f_main/f_menu_bar/f_settings_dialog/f_raycast/scene.hpp>
#include <fragments/f_main/f_menu_bar/f_settings_dialog/f_raycast/renderer2.hpp>
#include <utils.hpp>


using std::list;
using std::unique_ptr;


class RendererTest : public testing::Test {
  public:
    virtual void SetUp() override {}

    virtual void TearDown() override {}
};

TEST_F(RendererTest, findIntersections_r_singleRegion) {
  list<unique_ptr<Edge>> edges;

  Camera camera;
  camera.pos = Point(100, 100);
  camera.angle = DEG_TO_RAD(89.9);
  camera.height = 50;

  Size viewport;
  viewport.x = 10.0 * 320.0 / 240.0;
  viewport.y = 10.0;

  ConvexRegion* region = new ConvexRegion;

  Wall* wall = new Wall;
  edges.push_back(std::unique_ptr<Wall>(wall));
  wall->lseg = LineSegment(Point(1, 1), Point(200, 1));
  wall->texture = "light_bricks";
  region->edges.push_back(wall);

  wall = new Wall;
  edges.push_back(std::unique_ptr<Wall>(wall));
  wall->lseg = LineSegment(Point(200, 1), Point(200, 200));
  wall->texture = "light_bricks";
  region->edges.push_back(wall);

  wall = new Wall;
  edges.push_back(std::unique_ptr<Wall>(wall));
  wall->lseg = LineSegment(Point(200, 200), Point(0, 200));
  wall->texture = "light_bricks";
  region->edges.push_back(wall);

  wall = new Wall;
  edges.push_back(std::unique_ptr<Wall>(wall));
  wall->lseg = LineSegment(Point(0, 200), Point(1, 1));
  wall->texture = "light_bricks";
  region->edges.push_back(wall);

  //buildPolygon(*region);

  int screenW_px = 640;
  int screenH_px = 480;
  double hWorldUnitsInPx = screenW_px / viewport.x;
  double vWorldUnitsInPx = screenH_px / viewport.y;

  double F = computeF(viewport.x, camera.hFov);

  int screenX_px = 321;
  double projX_wd = static_cast<double>(screenX_px - screenW_px / 2) / hWorldUnitsInPx;
  Vec2f r(F, projX_wd);
  LineSegment ray(Point(0, 0), Point(r.x * 999.9, r.y * 999.9));

  CastResult result;
  findIntersections_r(camera, ray, *region, nullptr, result);

  result.intersections.sort([](const pIntersection_t& a, const pIntersection_t& b) {
    return a->distanceFromCamera < b->distanceFromCamera;
  });

  ASSERT_EQ(1, result.intersections.size());
}

TEST_F(RendererTest, findIntersections_r_nestedRegions) {
  list<unique_ptr<Edge>> edges;

  Camera camera;
  camera.pos = Point(10, 10);
  camera.angle = DEG_TO_RAD(45);
  camera.height = 50;

  Size viewport;
  viewport.x = 10.0 * 320.0 / 240.0;
  viewport.y = 10.0;

  ConvexRegion* region = new ConvexRegion;

  Wall* wall = new Wall;
  edges.push_back(std::unique_ptr<Wall>(wall));
  wall->lseg = LineSegment(Point(1, 1), Point(200, 1));
  wall->texture = "light_bricks";
  region->edges.push_back(wall);

  wall = new Wall;
  edges.push_back(std::unique_ptr<Wall>(wall));
  wall->lseg = LineSegment(Point(200, 1), Point(200, 200));
  wall->texture = "light_bricks";
  region->edges.push_back(wall);

  wall = new Wall;
  edges.push_back(std::unique_ptr<Wall>(wall));
  wall->lseg = LineSegment(Point(200, 200), Point(0, 200));
  wall->texture = "light_bricks";
  region->edges.push_back(wall);

  wall = new Wall;
  edges.push_back(std::unique_ptr<Wall>(wall));
  wall->lseg = LineSegment(Point(0, 200), Point(1, 1));
  wall->texture = "light_bricks";
  region->edges.push_back(wall);

  ConvexRegion* subregion = new ConvexRegion;

  wall = new Wall;
  edges.push_back(std::unique_ptr<Wall>(wall));
  wall->lseg = LineSegment(Point(51, 51), Point(150, 1));
  wall->texture = "light_bricks";
  subregion->edges.push_back(wall);

  wall = new Wall;
  edges.push_back(std::unique_ptr<Wall>(wall));
  wall->lseg = LineSegment(Point(150, 51), Point(150, 150));
  wall->texture = "light_bricks";
  subregion->edges.push_back(wall);

  wall = new Wall;
  edges.push_back(std::unique_ptr<Wall>(wall));
  wall->lseg = LineSegment(Point(150, 150), Point(50, 150));
  wall->texture = "light_bricks";
  subregion->edges.push_back(wall);

  wall = new Wall;
  edges.push_back(std::unique_ptr<Wall>(wall));
  wall->lseg = LineSegment(Point(50, 150), Point(51, 51));
  wall->texture = "light_bricks";
  subregion->edges.push_back(wall);

  region->children.push_back(unique_ptr<ConvexRegion>(subregion));

  //buildPolygon(*region);

  int screenW_px = 640;
  int screenH_px = 480;
  double hWorldUnitsInPx = screenW_px / viewport.x;
  double vWorldUnitsInPx = screenH_px / viewport.y;

  double F = computeF(viewport.x, camera.hFov);

  int screenX_px = 321;
  double projX_wd = static_cast<double>(screenX_px - screenW_px / 2) / hWorldUnitsInPx;
  Vec2f r(F, projX_wd);
  LineSegment ray(Point(0, 0), Point(r.x * 999.9, r.y * 999.9));

  CastResult result;
  findIntersections_r(camera, ray, *region, nullptr, result);

  result.intersections.sort([](const pIntersection_t& a, const pIntersection_t& b) {
    return a->distanceFromCamera < b->distanceFromCamera;
  });

  ASSERT_EQ(3, result.intersections.size());
}

TEST_F(RendererTest, findIntersections_r_joinedRegions) {
  list<unique_ptr<Edge>> edges;

  Camera camera;
  camera.pos = Point(100, 100);
  camera.angle = DEG_TO_RAD(0.1);
  camera.height = 50;

  Size viewport;
  viewport.x = 10.0 * 320.0 / 240.0;
  viewport.y = 10.0;

  ConvexRegion* rootRegion = new ConvexRegion;
  ConvexRegion* region1 = new ConvexRegion;
  ConvexRegion* region2 = new ConvexRegion;

  rootRegion->children.push_back(unique_ptr<ConvexRegion>(region1));
  rootRegion->children.push_back(unique_ptr<ConvexRegion>(region2));

  Wall* wall = new Wall;
  edges.push_back(std::unique_ptr<Wall>(wall));
  wall->lseg = LineSegment(Point(0, 0), Point(200, 1));
  wall->texture = "light_bricks";
  region1->edges.push_back(wall);

  JoiningEdge* je = new JoiningEdge;
  edges.push_back(std::unique_ptr<JoiningEdge>(je));
  je->lseg = LineSegment(Point(200, 1), Point(201, 201));
  je->topTexture = "light_bricks";
  je->bottomTexture = "dark_bricks";
  region1->edges.push_back(je);

  wall = new Wall;
  edges.push_back(std::unique_ptr<Wall>(wall));
  wall->lseg = LineSegment(Point(201, 201), Point(1, 200));
  wall->texture = "light_bricks";
  region1->edges.push_back(wall);

  wall = new Wall;
  edges.push_back(std::unique_ptr<Wall>(wall));
  wall->lseg = LineSegment(Point(1, 200), Point(0, 0));
  wall->texture = "light_bricks";
  region1->edges.push_back(wall);

  wall = new Wall;
  edges.push_back(std::unique_ptr<Wall>(wall));
  wall->lseg = LineSegment(Point(200, 1), Point(400, 2));
  wall->texture = "light_bricks";
  region2->edges.push_back(wall);

  wall = new Wall;
  edges.push_back(std::unique_ptr<Wall>(wall));
  wall->lseg = LineSegment(Point(400, 2), Point(401, 202));
  wall->texture = "light_bricks";
  region2->edges.push_back(wall);

  wall = new Wall;
  edges.push_back(std::unique_ptr<Wall>(wall));
  wall->lseg = LineSegment(Point(401, 202), Point(201, 201));
  wall->texture = "light_bricks";
  region2->edges.push_back(wall);

  region2->edges.push_back(je);
  je->regionA = region1;
  je->regionB = region2;

  //buildPolygon(*region);

  int screenW_px = 640;
  int screenH_px = 480;
  double hWorldUnitsInPx = screenW_px / viewport.x;
  double vWorldUnitsInPx = screenH_px / viewport.y;

  double F = computeF(viewport.x, camera.hFov);

  int screenX_px = 321;
  double projX_wd = static_cast<double>(screenX_px - screenW_px / 2) / hWorldUnitsInPx;
  Vec2f r(F, projX_wd);
  LineSegment ray(Point(0, 0), Point(r.x * 999.9, r.y * 999.9));

  CastResult result;
  findIntersections_r(camera, ray, *region1, nullptr, result);

  result.intersections.sort([](const pIntersection_t& a, const pIntersection_t& b) {
    return a->distanceFromCamera < b->distanceFromCamera;
  });

  ASSERT_EQ(2, result.intersections.size());
}
