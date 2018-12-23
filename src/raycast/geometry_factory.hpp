#ifndef __PROCALC_RAYCAST_GEOMETRY_FACTORY_HPP__
#define __PROCALC_RAYCAST_GEOMETRY_FACTORY_HPP__


#include <map>
#include <vector>
#include "raycast/game_object_factory.hpp"
#include "raycast/geometry.hpp"
#include "raycast/system_accessor.hpp"


class EntityManager;
class TimeService;
class Matrix;
class RootFactory;
namespace parser { struct Object; }

class GeometryFactory : public GameObjectFactory, private SystemAccessor {
  public:
    GeometryFactory(RootFactory& rootFactory, EntityManager& entityManager);

    const std::set<std::string>& types() const override;

    bool constructObject(const std::string& type, entityId_t entityId, parser::Object& obj,
      entityId_t parentId, const Matrix& parentTransform) override;

  private:
    RootFactory& m_rootFactory;

    std::map<Point, bool> m_endpoints;

    bool constructVRect(entityId_t, parser::Object& obj, entityId_t parentId,
      const Matrix& parentTransform);
    bool constructWallDecal(entityId_t entityId, parser::Object& obj, entityId_t parentId,
      const Matrix& parentTransform);
    bool constructWalls(parser::Object& obj, entityId_t parentId, const Matrix& parentTransform);
    bool constructFloorDecal(entityId_t entityId, parser::Object& obj, entityId_t parentId,
      const Matrix& parentTransform);
    bool constructBoundaries(entityId_t entityId, parser::Object& obj, entityId_t parentId,
      const Matrix& parentTransform);
    bool constructPortal(parser::Object& obj, entityId_t parentId, const Matrix& parentTransform);
    bool constructRegion_r(entityId_t entityId, parser::Object& obj, entityId_t parentId,
      const Matrix& parentTransform);
    bool constructRootRegion(parser::Object& obj);
    bool constructRegion(entityId_t entityId, parser::Object& obj, entityId_t parentId,
      const Matrix& parentTransform);
    bool constructPath(entityId_t entityId, parser::Object& obj, entityId_t parentId,
      const Matrix& parentTransform);
};


#endif
