#include <string>
#include <sstream>
#include <list>
#include <iterator>
#include "fragments/f_main/f_menu_bar/f_settings_dialog/f_raycast/scene.hpp"
#include "fragments/f_main/f_menu_bar/f_settings_dialog/f_raycast/geometry.hpp"
#include "fragments/f_main/f_menu_bar/f_settings_dialog/f_raycast/map_parser.hpp"
#include "exception.hpp"
#include "utils.hpp"


using std::unique_ptr;
using std::string;
using std::list;


//===========================================
// constructWalls
//===========================================
static void constructWalls(const parser::Object& obj, list<unique_ptr<Wall>>& walls) {
  for (unsigned int i = 0; i < obj.path.points.size(); ++i) {
    int j = i - 1;

    if (i == 0) {
      if (obj.path.closed) {
        j = obj.path.points.size() - 1;
      }
      else {
        continue;
      }
    }

    Wall* wall = new Wall;

    wall->lseg.A = obj.path.points[j];
    wall->lseg.B = obj.path.points[i];
    transform(wall->lseg, obj.transform);

    wall->texture = obj.dict.at("texture");

    walls.push_back(unique_ptr<Wall>(wall));
  }
}

//===========================================
// isTriangle
//===========================================
static bool isTriangle(const parser::Path& path) {
  return path.points.size() == 3 && path.closed;
}

//===========================================
// transformFromTriangle
//===========================================
static Matrix transformFromTriangle(const parser::Path& path) {
  if (!isTriangle(path)) {
    EXCEPTION("Path is not a triangle");
  }

  Point centre = (path.points[0] + path.points[1] + path.points[2]) / 3.0;
  Point mostDistantPoint;

  for (int i = 0; i < 3; ++i) {
    if (distance(path.points[i], centre) > distance(mostDistantPoint, centre)) {
      mostDistantPoint = path.points[i];
    }
  }

  Vec2f v = mostDistantPoint - centre;
  double a = atan(v.y / v.x) - PI;

  return Matrix(a, centre);
}

//===========================================
// constructCamera
//===========================================
static Camera* constructCamera(const parser::Object& obj) {
  Camera* camera = new Camera;
  camera->setTransform(obj.transform * transformFromTriangle(obj.path));

  return camera;
}

//===========================================
// constructSprite
//===========================================
static Sprite* constructSprite(const parser::Object& obj) {
  if (obj.dict.at("subtype") == "bad_guy") {
    BadGuy* sprite = new BadGuy;
    return sprite;
  }
  else if (obj.dict.at("subtype") == "ammo") {
    Ammo* sprite = new Ammo;
    Matrix m = transformFromTriangle(obj.path);
    sprite->setTransform(obj.transform * m);

    return sprite;
  }

  EXCEPTION("Error constructing sprite of unknown type");
}

//===========================================
// Scene::Scene
//===========================================
Scene::Scene(const string& mapFilePath) {
  list<parser::Object> objects = parser::parse(mapFilePath);
  for (auto it = objects.begin(); it != objects.end(); ++it) {
    addObject(*it);
  }

  viewport.x = 10.0 * 320.0 / 240.0; // TODO: Read from map file
  viewport.y = 10.0;
  wallHeight = 100.0;

  textures["light_bricks"] = QPixmap("data/light_bricks.png");
  textures["dark_bricks"] = QPixmap("data/dark_bricks.png");
  textures["floor"] = QPixmap("data/floor.png");
  textures["ceiling"] = QPixmap("data/ceiling.png");
  textures["ammo"] = QPixmap("data/ammo.png");
  textures["bad_guy"] = QPixmap("data/bad_guy.png");
}

//===========================================
// Scene::addObject
//===========================================
void Scene::addObject(const parser::Object& obj) {
  if (obj.dict.at("type") == "wall") {
    constructWalls(obj, walls);
  }
  else if (obj.dict.at("type") == "camera") {
    camera.reset(constructCamera(obj));
  }
  else if (obj.dict.at("type") == "sprite") {
    sprites.push_back(unique_ptr<Sprite>(constructSprite(obj)));
  }
}
