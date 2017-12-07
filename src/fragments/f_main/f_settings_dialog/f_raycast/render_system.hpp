#ifndef __PROCALC_FRAGMENTS_F_RAYCAST_RENDER_SYSTEM_HPP__
#define __PROCALC_FRAGMENTS_F_RAYCAST_RENDER_SYSTEM_HPP__


#include "fragments/f_main/f_settings_dialog/f_raycast/system.hpp"
#include "fragments/f_main/f_settings_dialog/f_raycast/renderer.hpp"


class QImage;
class EntityManager;
class Player;

class RenderSystem : public System {
  public:
    RenderSystem(EntityManager& entityManager, QImage& target);

    RenderGraph rg;

    void connectRegions();
    void render();

    virtual void update() override {}
    virtual void handleEvent(const GameEvent& event) override;
    virtual void handleEvent(const GameEvent& event,
      const std::set<entityId_t>& entities) override {}

    virtual void addComponent(pComponent_t component) override;
    virtual bool hasComponent(entityId_t entityId) const override;
    virtual Component& getComponent(entityId_t entityId) const override;
    virtual void removeEntity(entityId_t id) override;

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


#endif
