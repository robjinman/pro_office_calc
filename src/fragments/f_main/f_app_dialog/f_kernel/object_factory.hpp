#ifndef __PROCALC_FRAGMENTS_F_KERNEL_OBJECT_FACTORY_HPP__
#define __PROCALC_FRAGMENTS_F_KERNEL_OBJECT_FACTORY_HPP__


#include <map>
#include "raycast/game_object_factory.hpp"
#include "raycast/map_parser.hpp"


class EntityManager;
class AudioService;
class TimeService;
class Matrix;
class RootFactory;


namespace millennium_bug {


class ObjectFactory : public GameObjectFactory {
  public:
    ObjectFactory(RootFactory& rootFactory, EntityManager& entityManager, TimeService& timeService,
      AudioService& audioService);

    const std::set<std::string>& types() const override;

    bool constructObject(const std::string& type, entityId_t entityId, parser::Object& obj,
      entityId_t region, const Matrix& parentTransform) override;

    entityId_t region;
    Matrix parentTransform;
    std::map<entityId_t, parser::pObject_t> objects;
    bool firstPassComplete = false;

  private:
    bool constructComputerScreen(entityId_t entityId, parser::Object& obj, entityId_t parentId,
      const Matrix& parentTransform);

    bool constructCell(entityId_t entityId, parser::Object& obj, entityId_t parentId,
      const Matrix& parentTransform);

    RootFactory& m_rootFactory;
    EntityManager& m_entityManager;
    TimeService& m_timeService;
    AudioService& m_audioService;
};


}


#endif
