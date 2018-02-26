#ifndef __PROCALC_RAYCAST_MISC_FACTORY_HPP__
#define __PROCALC_RAYCAST_MISC_FACTORY_HPP__


#include "raycast/game_object_factory.hpp"


class EntityManager;
class AudioService;
class TimeService;
class Matrix;
class RootFactory;
namespace parser { class Object; }

class MiscFactory : public GameObjectFactory {
  public:
    MiscFactory(RootFactory& rootFactory, EntityManager& entityManager,
      AudioService& audioService, TimeService& timeService);

    const std::set<std::string>& types() const override;

    bool constructObject(const std::string& type, entityId_t entityId, parser::Object& obj,
      entityId_t parentId, const Matrix& parentTransform) override;

  private:
    RootFactory& m_rootFactory;
    EntityManager& m_entityManager;
    AudioService& m_audioService;
    TimeService& m_timeService;

    bool constructPlayerInventory();
    bool constructDoor(entityId_t entityId, parser::Object& obj, entityId_t parentId,
      const Matrix& parentTransform);
    bool constructSwitch(entityId_t entityId, parser::Object& obj, entityId_t parentId,
      const Matrix& parentTransform);
    bool constructElevator(entityId_t entityId, parser::Object& obj, entityId_t parentId,
      const Matrix& parentTransform);
    bool constructSpawnPoint(entityId_t entityId, parser::Object& obj, entityId_t parentId,
      const Matrix& parentTransform);
};


#endif
