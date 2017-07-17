#ifndef __PROCALC_FRAGMENTS_F_RAYCAST_SCENE_HPP__
#define __PROCALC_FRAGMENTS_F_RAYCAST_SCENE_HPP__


#include <string>
#include <list>
#include <map>
#include <memory>
#include <QImage>
#include "fragments/f_main/f_menu_bar/f_settings_dialog/f_raycast/scene_graph.hpp"
#include "fragments/f_main/f_menu_bar/f_settings_dialog/f_raycast/behaviour_system.hpp"


namespace tinyxml2 { class XMLElement; }
namespace parser { class Object; }


struct Tween {
  std::function<bool()> tick;
  std::function<void()> finish;
};

class EventSystem;

class Scene {
  public:
    Scene(EventSystem& eventSystem, const std::string& mapFilePath, double frameRate);

    SceneGraph sg;

    void update();
    void vRotateCamera(double da);
    void hRotateCamera(double da);
    void translateCamera(const Vec2f& dir);
    void jump();
    void addTween(const Tween& tween, const char* name = nullptr);

    ~Scene();

  private:
    EventSystem& m_eventSystem;
    std::list<int> m_eventIds;

    double m_frameRate;
    std::map<std::string, Tween> m_tweens;
    BehaviourSystem m_behaviourSystem;

    void buoyancy();
    void gravity();
};


#endif
