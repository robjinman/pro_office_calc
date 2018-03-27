#ifndef __PROCALC_FRAGMENTS_F_SERVER_ROOM_OBJECT_FACTORY_HPP__
#define __PROCALC_FRAGMENTS_F_SERVER_ROOM_OBJECT_FACTORY_HPP__


#include "raycast/game_object_factory.hpp"


class EntityManager;
class AudioService;
class TimeService;
class Matrix;
class RootFactory;
namespace parser { class Object; }


namespace youve_got_mail {


class ObjectFactory : public GameObjectFactory {
  public:
    ObjectFactory(RootFactory& rootFactory, EntityManager& entityManager,
      AudioService& audioService, TimeService& timeService);

    const std::set<std::string>& types() const override;

    bool constructObject(const std::string& type, entityId_t entityId, parser::Object& obj,
      entityId_t region, const Matrix& parentTransform) override;

  private:
    RootFactory& m_rootFactory;
    EntityManager& m_entityManager;
    AudioService& m_audioService;
    TimeService& m_timeService;

    bool constructBigScreen(entityId_t entityId, parser::Object& obj, entityId_t parentId,
      const Matrix& parentTransform);
};


}


#endif
