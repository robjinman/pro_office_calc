#ifndef __PROCALC_FRAGMENTS_F_RAYCAST_SCENE_HPP__
#define __PROCALC_FRAGMENTS_F_RAYCAST_SCENE_HPP__


#include <string>
#include <list>
#include <map>
#include <set>
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

class EntityManager;
class EventSystem;

class Scene : public System {
  public:
    Scene(EntityManager& entityManager, double frameRate);

    SceneGraph sg;

    void loadMap(const std::string& mapFilePath);
    void connectZones();

    virtual void update() override;
    virtual void handleEvent(const GameEvent& event) override;

    virtual void addComponent(pComponent_t component) override;
    virtual Component& getComponent(entityId_t entityId) const override;
    virtual void removeEntity(entityId_t id) override;

    std::set<entityId_t> getEntitiesInRadius(double radius) const;

    void vRotateCamera(double da);
    void hRotateCamera(double da);
    void translateCamera(const Vec2f& dir);
    void jump();
    void addTween(const Tween& tween, const char* name = nullptr);

    virtual ~Scene() override;

  private:
    EntityManager& m_entityManager;

    double m_frameRate;
    std::map<std::string, Tween> m_tweens;

    std::map<entityId_t, CRenderSpatial*> m_components;
    std::map<entityId_t, std::set<entityId_t>> m_entityChildren;

    bool isRoot(const CRenderSpatial& c) const;
    void removeEntity_r(entityId_t id);

    inline Zone& getCurrentZone() const {
      return dynamic_cast<Zone&>(*m_components.at(sg.player->currentRegion));
    }

    void buoyancy();
    void gravity();
};


#endif
