#ifndef __PROCALC_RAYCAST_GEOMETRY_FACTORY_HPP__
#define __PROCALC_RAYCAST_GEOMETRY_FACTORY_HPP__


#include <map>
#include "raycast/game_object_factory.hpp"
#include "raycast/geometry.hpp"


class EntityManager;
class AudioService;
class TimeService;
class Matrix;
class RootFactory;
namespace parser { class Object; }

class GeometryFactory : public GameObjectFactory {
  public:
    GeometryFactory(RootFactory& rootFactory, EntityManager& entityManager,
      AudioService& audioService, TimeService& timeService);

    virtual const std::set<std::string>& types() const override;

    virtual bool constructObject(const std::string& type, entityId_t entityId,
      const parser::Object& obj, entityId_t parentId, const Matrix& parentTransform) override;

    virtual ~GeometryFactory() override {}

  private:
    RootFactory& m_rootFactory;
    EntityManager& m_entityManager;
    AudioService& m_audioService;
    TimeService& m_timeService;

    std::map<Point, bool> m_endpoints;

    bool constructWallDecal(entityId_t entityId, const parser::Object& obj, entityId_t parentId,
      const Matrix& parentTransform);
    bool constructWalls(const parser::Object& obj, entityId_t parentId,
      const Matrix& parentTransform);
    bool constructFloorDecal(entityId_t entityId, const parser::Object& obj, entityId_t parentId,
      const Matrix& parentTransform);
    bool constructPlayer(const parser::Object& obj, entityId_t parentId,
      const Matrix& parentTransform);
    bool constructBoundaries(const parser::Object& obj, entityId_t parentId,
      const Matrix& parentTransform);
    bool constructRegion_r(entityId_t entityId, const parser::Object& obj, entityId_t parentId,
      const Matrix& parentTransform);
    bool constructRootRegion(const parser::Object& obj);
    bool constructRegion(entityId_t entityId, const parser::Object& obj, entityId_t parentId,
      const Matrix& parentTransform);
};


#endif
