#ifndef __PROCALC_FRAGMENTS_F_RAYCAST_AUDIO_SYSTEM_HPP__
#define __PROCALC_FRAGMENTS_F_RAYCAST_AUDIO_SYSTEM_HPP__


#include <map>
#include "fragments/f_main/f_menu_bar/f_settings_dialog/f_raycast/system.hpp"


struct CAudio : public Component {
  CAudio(entityId_t entityId)
    : Component(entityId, ComponentKind::C_AUDIO) {}
};

typedef std::unique_ptr<CAudio> pCAudio_t;

class EntityManager;

class AudioSystem : public System {
  public:
    AudioSystem(EntityManager& entityManager)
      : m_entityManager(entityManager) {}

    virtual void update() override;
    virtual void handleEvent(const GameEvent& event) override;
    virtual void addComponent(pComponent_t component) override;
    virtual bool hasComponent(entityId_t entityId) const override;
    virtual Component& getComponent(entityId_t entityId) const override;
    virtual void removeEntity(entityId_t id) override;

    virtual ~AudioSystem() override {}

  private:
    EntityManager& m_entityManager;
    std::map<entityId_t, pCAudio_t> m_components;
};


#endif
