#ifndef __PROCALC_FRAGMENTS_F_KERNEL_OBJECT_FACTORY_HPP__
#define __PROCALC_FRAGMENTS_F_KERNEL_OBJECT_FACTORY_HPP__


#include "raycast/game_object_factory.hpp"


class EntityManager;
class AudioService;
class TimeService;
class Matrix;
class RootFactory;
namespace parser { struct Object; }


namespace millennium_bug {


class ObjectFactory : public GameObjectFactory {
  public:
    ObjectFactory(RootFactory& rootFactory, EntityManager& entityManager, TimeService& timeService,
      AudioService& audioService);

    const std::set<std::string>& types() const override;

    bool constructObject(const std::string& type, entityId_t entityId, parser::Object& obj,
      entityId_t region, const Matrix& parentTransform) override;

  private:
    RootFactory& m_rootFactory;
    EntityManager& m_entityManager;
    TimeService& m_timeService;
    AudioService& m_audioService;

    bool constructComputerScreen(entityId_t entityId, parser::Object& obj, entityId_t parentId,
      const Matrix& parentTransform);
};


}


#endif