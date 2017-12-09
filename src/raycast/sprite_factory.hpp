#ifndef __PROCALC_RAYCAST_SPRITE_FACTORY_HPP__
#define __PROCALC_RAYCAST_SPRITE_FACTORY_HPP__


#include "raycast/game_object_factory.hpp"


class EntityManager;
class AudioService;
class TimeService;
class Matrix;
class RootFactory;
namespace parser { class Object; }

class SpriteFactory : public GameObjectFactory {
  public:
    SpriteFactory(RootFactory& rootFactory, EntityManager& entityManager,
      AudioService& audioService, TimeService& timeService);

    virtual const std::set<std::string>& types() const override;

    virtual bool constructObject(const std::string& type, entityId_t entityId,
      const parser::Object& obj, entityId_t region, const Matrix& parentTransform) override;

    virtual ~SpriteFactory() override {}

  private:
    RootFactory& m_rootFactory;
    EntityManager& m_entityManager;
    AudioService& m_audioService;
    TimeService& m_timeService;

    bool constructAmmo(entityId_t entityId, const parser::Object& obj, entityId_t parentId,
      const Matrix& parentTransform);
    bool constructBadGuy(entityId_t entityId, const parser::Object& obj, entityId_t parentId,
      const Matrix& parentTransform);
};


#endif
