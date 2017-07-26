#ifndef __PROCALC_FRAGMENTS_F_RAYCAST_ANIMATION_SYSTEM_HPP__
#define __PROCALC_FRAGMENTS_F_RAYCAST_ANIMATION_SYSTEM_HPP__


#include <map>
#include <vector>
#include <QRectF>
#include "fragments/f_main/f_menu_bar/f_settings_dialog/f_raycast/system.hpp"


struct AnimationFrame {
  std::vector<QRectF> texViews;
};

class Animation {
  public:
    int fps = 0;
    std::vector<AnimationFrame> frames;

    void update();
    const AnimationFrame& currentFrame() const {
      return frames[m_currentFrameIdx];
    }

  private:
    double m_elapsed = 0.0;
    int m_currentFrameIdx = 0;
};

struct CAnimation : public Component {
  CAnimation(entityId_t entityId)
    : Component(entityId, ComponentKind::C_ANIMATION) {}

  std::map<std::string, Animation> animations;
};

class EntityManager;

class AnimationSystem : public System {
  public:
    AnimationSystem(EntityManager& entityManager)
      : m_entityManager(entityManager) {}

    virtual void update() override;
    virtual void handleEvent(const GameEvent& event) override;
    virtual void addComponent(pComponent_t component) override;
    virtual Component& getComponent(entityId_t entityId) const override;
    virtual void removeEntity(entityId_t id) override;

    virtual ~AnimationSystem() override {}

  private:
    EntityManager& m_entityManager;
    std::map<entityId_t, CAnimation*> m_components;
};


#endif
