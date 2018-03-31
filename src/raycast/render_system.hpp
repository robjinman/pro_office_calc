#ifndef __PROCALC_RAYCAST_RENDER_SYSTEM_HPP__
#define __PROCALC_RAYCAST_RENDER_SYSTEM_HPP__


#include "raycast/system.hpp"
#include "raycast/renderer.hpp"


class QImage;
class EntityManager;
class Player;

class RenderSystem : public System {
  public:
    RenderSystem(EntityManager& entityManager, QImage& target);

    RenderGraph rg;

    void connectRegions();
    void render();

    void update() override {}
    void handleEvent(const GameEvent& event) override;
    void handleEvent(const GameEvent& event, const std::set<entityId_t>& entities) override {}

    void addComponent(pComponent_t component) override;
    bool hasComponent(entityId_t entityId) const override;
    CRender& getComponent(entityId_t entityId) const override;
    void removeEntity(entityId_t id) override;

    inline const std::set<entityId_t>& children(entityId_t entityId) const;

  private:
    EntityManager& m_entityManager;
    QImage& m_target;
    Renderer m_renderer;

    std::map<entityId_t, CRender*> m_components;
    std::map<entityId_t, std::set<entityId_t>> m_entityChildren;

    bool isRoot(const CRender& c) const;
    void removeEntity_r(entityId_t id);
    void crossRegions(RenderGraph& rg, entityId_t entityId, entityId_t oldZone, entityId_t newZone);
};

inline const std::set<entityId_t>& RenderSystem::children(entityId_t entityId) const {
  static const std::set<entityId_t> emptySet;

  auto it = m_entityChildren.find(entityId);
  return it != m_entityChildren.end() ? it->second : emptySet;
}


#endif
