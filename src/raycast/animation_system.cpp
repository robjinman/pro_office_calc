#include "raycast/animation_system.hpp"
#include "raycast/render_system.hpp"
#include "raycast/entity_manager.hpp"


using std::vector;
using std::set;
using std::pair;


//===========================================
// constructFrames
//===========================================
vector<AnimationFrame> constructFrames(int W, int H, const vector<int>& rows) {
  double w = 1.0 / W;
  double h = 1.0 / H;

  vector<AnimationFrame> frames;
  for (int f : rows) {
    AnimationFrame frame;

    for (int v = 0; v < W; ++v) {
      frame.texViews.push_back(QRectF(w * v, h * f, w, h));
    }

    frames.push_back(frame);
  }

  return frames;
}

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
//
// Returns true when the animation finishes
//===========================================
bool Animation::update() {
  if (frames.empty()) {
    return false;
  }

  if (m_state == AnimState::RUNNING) {
    double sim_dt = 1.0 / m_gameFrameRate;
    double anim_dt = m_duration / frames.size();

    // Since first frame
    m_elapsed += sim_dt;

    unsigned int next = floor(m_elapsed / anim_dt);

    if (next == frames.size()) {
      if (m_loop) {
        m_currentFrameIdx = 0;
        m_elapsed = 0;
      }
      else {
        m_state = AnimState::STOPPED;
        return true;
      }
    }
    else {
      m_currentFrameIdx = next;
    }
  }

  return false;
}

//===========================================
// AnimationSystem::playAnimation
//===========================================
void AnimationSystem::playAnimation(entityId_t entityId, const std::string& name, bool loop) {
  auto it = m_components.find(entityId);
  if (it != m_components.end()) {
    CAnimation& component = *it->second;

    auto jt = component.m_animations.find(name);
    if (jt != component.m_animations.end()) {
      pair<pAnimation_t, pAnimation_t>& anims = jt->second;

      stopAnimation(entityId, false);

      if (anims.first) {
        anims.first->start(loop);
      }
      if (anims.second) {
        anims.second->start(loop);
      }

      component.m_active = name;
    }
  }

  RenderSystem& renderSystem = m_entityManager.system<RenderSystem>(ComponentKind::C_RENDER);
  auto& children = renderSystem.children(entityId);
  for (entityId_t child : children) {
    playAnimation(child, name, loop);
  }
}

//===========================================
// AnimationSystem::stopAnimation
//===========================================
void AnimationSystem::stopAnimation(entityId_t entityId, bool recurseIntoChildren) {
  auto it = m_components.find(entityId);
  if (it != m_components.end()) {
    CAnimation& component = *it->second;

    if (component.m_active != "") {
      auto& anims = component.m_animations[component.m_active];
      if (anims.first) {
        anims.first->stop();
      }
      if (anims.second) {
        anims.second->stop();
      }
    }
  }

  if (recurseIntoChildren) {
    RenderSystem& renderSystem = m_entityManager.system<RenderSystem>(ComponentKind::C_RENDER);
    auto& children = renderSystem.children(entityId);
    for (entityId_t child : children) {
      stopAnimation(child, recurseIntoChildren);
    }
  }
}

//===========================================
// AnimationSystem::updateAnimations
//===========================================
void AnimationSystem::updateAnimation(CAnimation& c, int which) {
  if (c.m_active == "") {
    return;
  }

  entityId_t entityId = c.entityId();

  Animation* anim = nullptr;
  auto& anims = c.m_animations[c.m_active];

  if (which == 0) {
    anim = anims.first.get();
  }
  else if (which == 1) {
    anim = anims.second.get();
  }

  if (anim != nullptr) {
    bool justFinished = anim->update();

    RenderSystem& renderSystem = m_entityManager.system<RenderSystem>(ComponentKind::C_RENDER);
    CRender& render = renderSystem.getComponent(entityId);

    if (render.kind == CRenderKind::SPRITE) {
      CSprite& sprite = dynamic_cast<CSprite&>(render);
      sprite.texViews = anim->currentFrame().texViews;
    }
    else if (render.kind == CRenderKind::OVERLAY) {
      COverlay& overlay = dynamic_cast<COverlay&>(render);

      if (overlay.kind == COverlayKind::IMAGE) {
        CImageOverlay& imgOverlay = dynamic_cast<CImageOverlay&>(render);
        imgOverlay.texRect = anim->currentFrame().texViews[0];
      }
    }
    else if (render.kind == CRenderKind::WALL_DECAL) {
      CWallDecal& decal = dynamic_cast<CWallDecal&>(render);
      decal.texRect = anim->currentFrame().texViews[0];
    }
    else if (render.kind == CRenderKind::WALL) {
      CWall& wall = dynamic_cast<CWall&>(render);
      wall.texRect = anim->currentFrame().texViews[0];
    }
    else if (render.kind == CRenderKind::JOIN) {
      CJoin& join = dynamic_cast<CJoin&>(render);

      if (which == 0) {
        join.bottomTexRect = anim->currentFrame().texViews[0];
      }
      else {
        join.topTexRect = anim->currentFrame().texViews[0];
      }
    }
    else if (render.kind == CRenderKind::REGION) {
      CRegion& region = dynamic_cast<CRegion&>(render);

      if (which == 0) {
        region.floorTexRect = anim->currentFrame().texViews[0];
      }
      else {
        region.ceilingTexRect = anim->currentFrame().texViews[0];
      }
    }

    if (anim->state() == AnimState::STOPPED) {
      c.m_active = "";
    }

    if (justFinished) {
      m_entityManager.fireEvent(EAnimationFinished(entityId, anim->name), { entityId });
    }
  }
}

//===========================================
// AnimationSystem::update
//===========================================
void AnimationSystem::update() {
  for (auto it = m_components.begin(); it != m_components.end(); ++it) {
    updateAnimation(*it->second, 0);
    updateAnimation(*it->second, 1);
  }
}

//===========================================
// AnimationSystem::addComponent
//===========================================
void AnimationSystem::addComponent(pComponent_t component) {
  CAnimation* p = dynamic_cast<CAnimation*>(component.release());
  m_components.insert(std::make_pair(p->entityId(), pCAnimation_t(p)));
}

//===========================================
// AnimationSystem::hasComponent
//===========================================
bool AnimationSystem::hasComponent(entityId_t entityId) const {
  return m_components.find(entityId) != m_components.end();
}

//===========================================
// AnimationSystem::getComponent
//===========================================
CAnimation& AnimationSystem::getComponent(entityId_t entityId) const {
  return *m_components.at(entityId);
}

//===========================================
// AnimationSystem::removeEntity
//===========================================
void AnimationSystem::removeEntity(entityId_t id) {
  m_components.erase(id);
}
