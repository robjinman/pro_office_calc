#ifndef __PROCALC_RAYCAST_SPRITE_FACTORY_HPP__
#define __PROCALC_RAYCAST_SPRITE_FACTORY_HPP__


#include "raycast/game_object_factory.hpp"
#include "raycast/system_accessor.hpp"


class EntityManager;
class AudioService;
class TimeService;
class Matrix;
class RootFactory;
namespace parser { struct Object; }

class SpriteFactory : public GameObjectFactory, private SystemAccessor {
  public:
    SpriteFactory(RootFactory& rootFactory, EntityManager& entityManager,
      AudioService& audioService, TimeService& timeService);

    const std::set<std::string>& types() const override;

    bool constructObject(const std::string& type, entityId_t entityId, parser::Object& obj,
      entityId_t region, const Matrix& parentTransform) override;

  private:
    RootFactory& m_rootFactory;
    EntityManager& m_entityManager;
    AudioService& m_audioService;
    TimeService& m_timeService;

    bool constructSprite(entityId_t entityId, parser::Object& obj, entityId_t parentId,
      const Matrix& parentTransform);
    bool constructAmmo(entityId_t entityId, parser::Object& obj, entityId_t parentId,
      const Matrix& parentTransform);
    bool constructHealthPack(entityId_t entityId, parser::Object& obj, entityId_t parentId,
      const Matrix& parentTransform);
    bool constructBadGuy(entityId_t entityId, parser::Object& obj, entityId_t parentId,
      const Matrix& parentTransform);
    bool constructCivilian(entityId_t entityId, parser::Object& obj, entityId_t parentId,
      const Matrix& parentTransform);

    void setupCivilianFocus(entityId_t entityId, const parser::Object& obj);
    void setupCivilianAnimations(entityId_t entityId);
    void setupCivilianDamage(entityId_t entityId);
    void setupCivilianEvents(entityId_t entityId);
    void setupCivilianAgent(entityId_t entityId, const parser::Object& obj);
    void setupCivilianInventory(entityId_t entityId, const parser::Object& obj,
      const Matrix& parentTransform);
    void onCivilianDamage(entityId_t entityId);

    void setupBadGuyAnimations(entityId_t entityId);
    void setupBadGuySpawning(entityId_t entityId, const parser::Object& obj, entityId_t parentId,
      const Matrix& parentTransform);
    void setupBadGuyDamage(entityId_t entityId);
    void setupBadGuyEvents(entityId_t entityId);
    void setupBadGuyAgent(entityId_t entityId, const parser::Object& obj);
    void onBadGuyDamage(entityId_t entityId);
};


#endif
