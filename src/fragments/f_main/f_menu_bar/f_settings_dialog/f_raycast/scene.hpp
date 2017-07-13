#ifndef __PROCALC_FRAGMENTS_F_RAYCAST_SCENE_HPP__
#define __PROCALC_FRAGMENTS_F_RAYCAST_SCENE_HPP__


#include <string>
#include <list>
#include <map>
#include <memory>
#include <QImage>
#include "fragments/f_main/f_menu_bar/f_settings_dialog/f_raycast/scene_graph.hpp"


namespace tinyxml2 { class XMLElement; }
namespace parser { class Object; }


struct Tween {
  std::function<bool()> tick;
  std::function<void()> finish;
};

class Scene {
  public:
    Scene(const std::string& mapFilePath, double frameRate);

    SceneGraph sg;

    void update();
    void vRotateCamera(double da);
    void hRotateCamera(double da);
    void translateCamera(const Vec2f& dir);
    void jump();
    void addTween(const Tween& tween, const char* name = nullptr);

  private:
    double m_frameRate;
    std::map<std::string, Tween> m_tweens;

    void gravity();
};


#endif
