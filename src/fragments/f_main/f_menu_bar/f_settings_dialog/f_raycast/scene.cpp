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


static void constructWalls(const parser::Object& obj, list<pLineSegment_t>& walls) {
  for (auto it = std::next(obj.path.points.begin()); it != obj.path.points.end(); ++it) {
    LineSegment* lseg = new LineSegment(*std::prev(it), *it);
    lseg->transform(obj.transform);

    walls.push_back(pLineSegment_t(lseg));
  }
}

static Camera* constructCamera(const parser::Object& obj) {
  Camera* camera = new Camera;
  camera->setTransform(obj.transform);

  return camera;
}

void Scene::addObject(const parser::Object& obj) {
  std::cout << "Type=" << obj.dict.at("type") << "\n";

  if (obj.dict.at("type") == "wall") {
    constructWalls(obj, walls);
  }
  else if (obj.dict.at("type") == "camera") {
    camera.reset(constructCamera(obj));
  }
}

Scene::Scene(const std::string& mapFilePath) {
  list<parser::Object> objects = parser::parse(mapFilePath);
  for (auto it = objects.begin(); it != objects.end(); ++it) {
    addObject(*it);
  }

  viewport.x = 8.0;
  viewport.y = 8.0;
  wallHeight = 10.0;

  texture.reset(new QPixmap("data/wall.png"));
}
