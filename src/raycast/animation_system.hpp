#ifndef __PROCALC_RAYCAST_ANIMATION_SYSTEM_HPP__
#define __PROCALC_RAYCAST_ANIMATION_SYSTEM_HPP__


#include <map>
#include <vector>
#include <QRectF>
#include "raycast/system.hpp"
#include "exception.hpp"


struct AnimationFrame {
  std::vector<QRectF> texViews;
};

enum class AnimState {
  STOPPED,
  RUNNING
};

class Animation {
  public:
    Animation(const std::string& name, double gameFrameRate, double duration,
      const std::vector<AnimationFrame>& frames)
      : name(name),
        frames(frames),
        m_gameFrameRate(gameFrameRate),
        m_duration(duration) {}

    std::string name;
    std::vector<AnimationFrame> frames;

    void start(bool loop);
    void stop();
    bool update();

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

typedef std::unique_ptr<Animation> pAnimation_t;

struct EAnimationFinished : public GameEvent {
  explicit EAnimationFinished(entityId_t entityId, const std::string& animName)
    : GameEvent("animation_finished"),
      entityId(entityId),
      animName(animName) {}

  entityId_t entityId;
  std::string animName;

  virtual ~EAnimationFinished() override {}
};

class CAnimation : public Component {
  friend class AnimationSystem;

  public:
    CAnimation(entityId_t entityId)
      : Component(entityId, ComponentKind::C_ANIMATION) {}

    // For CRegions, anim1 and anim2 are for floors and ceilings, respectively.
    // For CJoins, anim1 and anim2 are for the bottom wall and the top wall, respectively.
    void addAnimation(pAnimation_t anim1, pAnimation_t anim2 = nullptr) {
      m_animations[anim1->name].first = std::move(anim1);

      if (anim2) {
        if (anim2->name != anim1->name) {
          EXCEPTION("Pair of animations '" << anim1->name << "' and '" << anim2->name
            << "' do not have same name");
        }
        m_animations[anim2->name].second = std::move(anim2);
      }
    }

  private:
    std::string m_active;
    std::map<std::string, std::pair<pAnimation_t, pAnimation_t>> m_animations;
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
    CAnimation& getComponent(entityId_t entityId) const override;
    void removeEntity(entityId_t id) override;

  private:
    EntityManager& m_entityManager;
    double m_frameRate;
    std::map<entityId_t, pCAnimation_t> m_components;

    void updateAnimation(CAnimation& c, int which);
};


std::vector<AnimationFrame> constructFrames(int W, int H, const std::vector<int>& rows);


#endif
