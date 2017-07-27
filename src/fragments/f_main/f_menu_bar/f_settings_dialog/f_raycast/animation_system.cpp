#include "fragments/f_main/f_menu_bar/f_settings_dialog/f_raycast/animation_system.hpp"
#include "fragments/f_main/f_menu_bar/f_settings_dialog/f_raycast/render_system.hpp"
#include "fragments/f_main/f_menu_bar/f_settings_dialog/f_raycast/entity_manager.hpp"


//===========================================
// Animation::start
//===========================================
void Animation::start(bool loop) {
  m_loop = loop;
  m_state = AnimState::RUNNING;
  m_elapsed = 0;
  m_currentFrameIdx = 0;
}

//===========================================
// Animation::stop
//===========================================
void Animation::stop() {
  m_state = AnimState::STOPPED;
  m_elapsed = 0;
  m_currentFrameIdx = 0;
}

//===========================================
// Animation::update
//===========================================
void Animation::update() {
  if (frames.empty()) {
    return;
  }

  if (m_state == AnimState::RUNNING) {
    double sim_dt = 1.0 / m_gameFrameRate;
    double anim_dt = m_duration / frames.size();

    // Since first frame
    m_elapsed += sim_dt;

    m_currentFrameIdx = floor(m_elapsed / anim_dt);

    if (m_currentFrameIdx == frames.size()) {
      m_currentFrameIdx = 0;
      m_elapsed = 0;

      if (!m_loop) {
        m_state = AnimState::STOPPED;
      }
    }
  }
}

//===========================================
// AnimationSystem::playAnimation
//===========================================
void AnimationSystem::playAnimation(entityId_t entityId, const std::string& name, bool loop) {
  CAnimation& component = *m_components.at(entityId);
  Animation& anim = component.animations.at(name);
  anim.start(loop);
  component.active = &anim;
}

//===========================================
// AnimationSystem::stopAnimation
//===========================================
void AnimationSystem::stopAnimation(entityId_t entityId) {
  CAnimation& component = *m_components.at(entityId);
  if (component.active != nullptr) {
    component.active->stop();
  }
}

//===========================================
// AnimationSystem::update
//===========================================
void AnimationSystem::update() {
  for (auto it = m_components.begin(); it != m_components.end(); ++it) {
    Animation* anim = it->second->active;
    if (anim != nullptr) {
      anim->update();

      RenderSystem& renderSystem = m_entityManager.system<RenderSystem>(ComponentKind::C_RENDER);
      CRender& c = dynamic_cast<CRender&>(renderSystem.getComponent(it->first));

      if (c.kind == CRenderKind::SPRITE) {
        CSprite& sprite = dynamic_cast<CSprite&>(c);
        sprite.texViews = anim->currentFrame().texViews;
      }

      if (anim->state() == AnimState::STOPPED) {
        it->second->active = nullptr;
      }
    }
  }
}

//===========================================
// AnimationSystem::handleEvent
//===========================================
void AnimationSystem::handleEvent(const GameEvent& event) {}

//===========================================
// AnimationSystem::addComponent
//===========================================
void AnimationSystem::addComponent(pComponent_t component) {
  CAnimation* p = dynamic_cast<CAnimation*>(component.release());
  m_components.insert(std::make_pair(p->entityId(), pCAnimation_t(p)));
}

//===========================================
// AnimationSystem::getComponent
//===========================================
Component& AnimationSystem::getComponent(entityId_t entityId) const {
  return *m_components.at(entityId);
}

//===========================================
// AnimationSystem::removeEntity
//===========================================
void AnimationSystem::removeEntity(entityId_t id) {
  m_components.erase(id);
}
