#ifndef __PROCALC_RAYCAST_ANIMATION_SYSTEM_HPP__
#define __PROCALC_RAYCAST_ANIMATION_SYSTEM_HPP__


#include <map>
#include <vector>
#include <QRectF>
#include "raycast/system.hpp"


struct AnimationFrame {
  std::vector<QRectF> texViews;
};

enum class AnimState {
  STOPPED,
  RUNNING
};

class Animation {
  public:
    Animation(double gameFrameRate, double duration, const std::vector<AnimationFrame>& frames)
      : frames(frames),
        m_gameFrameRate(gameFrameRate),
        m_duration(duration) {}

    std::vector<AnimationFrame> frames;

    void start(bool loop);
    void stop();
    void update();

    const AnimationFrame& currentFrame() const {
      return frames[m_currentFrameIdx];
    }

    AnimState state() const {
      return m_state;
    }

  private:
    const double m_gameFrameRate;
    const double m_duration;

    double m_elapsed = 0.0;
    unsigned int m_currentFrameIdx = 0;
    AnimState m_state = AnimState::STOPPED;
    bool m_loop = false;
};

struct CAnimation : public Component {
  CAnimation(entityId_t entityId)
    : Component(entityId, ComponentKind::C_ANIMATION) {}

  Animation* active = nullptr;
  std::map<std::string, Animation> animations;
};

typedef std::unique_ptr<CAnimation> pCAnimation_t;

class EntityManager;

class AnimationSystem : public System {
  public:
    AnimationSystem(EntityManager& entityManager, double frameRate)
      : m_entityManager(entityManager),
        m_frameRate(frameRate) {}

    void playAnimation(entityId_t entityId, const std::string& anim, bool loop);
    void stopAnimation(entityId_t entityId);

    void update() override;
    void handleEvent(const GameEvent& event) override {}
    void handleEvent(const GameEvent& event, const std::set<entityId_t>& entities) override {}

    void addComponent(pComponent_t component) override;
    bool hasComponent(entityId_t entityId) const override;
    Component& getComponent(entityId_t entityId) const override;
    void removeEntity(entityId_t id) override;

  private:
    EntityManager& m_entityManager;
    double m_frameRate;
    std::map<entityId_t, pCAnimation_t> m_components;
};


std::vector<AnimationFrame> constructFrames(int W, int H, const std::vector<int>& rows);


#endif
