#ifndef __PROCALC_FRAGMENTS_F_RAYCAST_SCENE_HPP__
#define __PROCALC_FRAGMENTS_F_RAYCAST_SCENE_HPP__


#include <string>
#include <memory>
#include <list>
#include <QPixmap>
#include "fragments/f_main/f_menu_bar/f_settings_dialog/f_raycast/geometry.hpp"
#include "fragments/f_main/f_menu_bar/f_settings_dialog/f_raycast/camera.hpp"


namespace tinyxml2 { class XMLElement; }

class Scene {
  public:
    Scene(const std::string& mapFilePath);

    std::unique_ptr<Camera> camera;
    std::list<std::unique_ptr<LineSegment>> walls;
    Vec2f viewport;
    double wallHeight;
    std::unique_ptr<QPixmap> texture;

  private:
    void addFromSvgElement(const tinyxml2::XMLElement& e);
};


#endif
