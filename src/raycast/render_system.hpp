#ifndef __PROCALC_RAYCAST_RENDER_SYSTEM_HPP__
#define __PROCALC_RAYCAST_RENDER_SYSTEM_HPP__


#include "raycast/system.hpp"
#include "raycast/renderer.hpp"


class QImage;
class EntityManager;
class Player;
class AppConfig;
class Camera;

class RenderSystem : public System {
  public:
    RenderSystem(const AppConfig& appConfig, EntityManager& entityManager, QImage& target);

    RenderGraph rg;

    inline const Size& viewport() const;
    inline const Size& viewport_px() const;
    inline Size worldUnit_px() const;

    inline void setCamera(const Camera* cam);

    void connectRegions();
    void render();

    void update() override {}
    void handleEvent(const GameEvent& event) override;
    void handleEvent(const GameEvent&, const std::set<entityId_t>&) override {}

    void addComponent(pComponent_t component) override;
    bool hasComponent(entityId_t entityId) const override;
    CRender& getComponent(entityId_t entityId) const override;
    void removeEntity(entityId_t id) override;

    inline const std::set<entityId_t>& children(entityId_t entityId) const;

    double textOverlayWidth(const CTextOverlay& overlay) const;
    void centreTextOverlay(CTextOverlay& overlay) const;

  private:
    const AppConfig& m_appConfig;
    EntityManager& m_entityManager;
    Renderer m_renderer;

    std::map<entityId_t, CRender*> m_components;
    std::map<entityId_t, std::set<entityId_t>> m_entityChildren;

    bool isRoot(const CRender& c) const;
    void removeEntity_r(entityId_t id);
    void crossRegions(RenderGraph& rg, entityId_t entityId, entityId_t oldZone, entityId_t newZone);
};

//===========================================
// RenderSystem::viewport
//===========================================
inline const Size& RenderSystem::viewport() const {
  return m_renderer.viewport();
}

//===========================================
// RenderSystem::viewport_px
//===========================================
inline const Size& RenderSystem::viewport_px() const {
  return m_renderer.viewport_px();
}

//===========================================
// RenderSystem::worldUnit_px
//===========================================
inline Size RenderSystem::worldUnit_px() const {
  return m_renderer.worldUnit_px();
}

//===========================================
// RenderSystem::setCamera
//===========================================
inline void RenderSystem::setCamera(const Camera* cam) {
  m_renderer.setCamera(cam);
}

//===========================================
// RenderSystem::children
//===========================================
inline const std::set<entityId_t>& RenderSystem::children(entityId_t entityId) const {
  static const std::set<entityId_t> emptySet;

  auto it = m_entityChildren.find(entityId);
  return it != m_entityChildren.end() ? it->second : emptySet;
}


#endif
