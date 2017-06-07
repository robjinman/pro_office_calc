#ifndef __PROCALC_FRAGMENTS_F_RAYCAST_SCENE_HPP__
#define __PROCALC_FRAGMENTS_F_RAYCAST_SCENE_HPP__


#include <string>
#include <list>
#include <map>
#include <QPixmap>
#include "fragments/f_main/f_menu_bar/f_settings_dialog/f_raycast/geometry.hpp"
#include "fragments/f_main/f_menu_bar/f_settings_dialog/f_raycast/camera.hpp"


namespace tinyxml2 { class XMLElement; }
namespace parser { class Object; }

struct Wall {
  LineSegment lseg;
  std::string texture;
};

class Scene {
  public:
    Scene(const std::string& mapFilePath);

    Camera camera;
    std::list<Wall> walls;
    Vec2f viewport;
    double wallHeight;
    std::map<std::string, QPixmap> textures;

  private:
    void addObject(const parser::Object& obj);
};


#endif
