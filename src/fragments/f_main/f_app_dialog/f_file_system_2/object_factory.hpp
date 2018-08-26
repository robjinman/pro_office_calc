#ifndef __PROCALC_FRAGMENTS_F_FILE_SYSTEM_2_OBJECT_FACTORY_HPP__
#define __PROCALC_FRAGMENTS_F_FILE_SYSTEM_2_OBJECT_FACTORY_HPP__


#include "raycast/game_object_factory.hpp"
#include "raycast/system_accessor.hpp"


class EntityManager;
class TimeService;
class Matrix;
class RootFactory;
namespace parser { struct Object; }


namespace t_minus_two_minutes {


class ObjectFactory : public GameObjectFactory, private SystemAccessor {
  public:
    ObjectFactory(RootFactory& rootFactory, EntityManager& entityManager, TimeService& timeService);

    const std::set<std::string>& types() const override;

    bool constructObject(const std::string& type, entityId_t entityId, parser::Object& obj,
      entityId_t region, const Matrix& parentTransform) override;

  private:
    bool constructCovfefe(entityId_t entityId, parser::Object& obj, entityId_t region,
      const Matrix& parentTransform);

    bool constructCog(entityId_t entityId, parser::Object& obj, entityId_t region,
      const Matrix& parentTransform);

    bool constructSmoke(entityId_t entityId, parser::Object& obj, entityId_t region,
      const Matrix& parentTransform);

    bool constructBridgeSection(entityId_t entityId, parser::Object& obj, entityId_t region,
      const Matrix& parentTransform);

    RootFactory& m_rootFactory;
    EntityManager& m_entityManager;
    TimeService& m_timeService;
};


}


#endif
