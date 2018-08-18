#ifndef __PROCALC_FRAGMENTS_F_DOOMSWEEPER_OBJECT_FACTORY_HPP__
#define __PROCALC_FRAGMENTS_F_DOOMSWEEPER_OBJECT_FACTORY_HPP__


#include <map>
#include <array>
#include "raycast/game_object_factory.hpp"
#include "raycast/map_parser.hpp"


class EntityManager;
class TimeService;
class Matrix;
class RootFactory;


namespace doomsweeper {


class ObjectFactory : public GameObjectFactory {
  public:
    ObjectFactory(RootFactory& rootFactory, EntityManager& entityManager, TimeService& timeService);

    const std::set<std::string>& types() const override;

    bool constructObject(const std::string& type, entityId_t entityId, parser::Object& obj,
      entityId_t region, const Matrix& parentTransform) override;

    struct CellDoors {
      typedef std::array<entityId_t, 4> idList_t;
      idList_t ids = {{-1, -1, -1, -1}};

      entityId_t& north = ids[0];
      entityId_t& east = ids[1];
      entityId_t& south = ids[2];
      entityId_t& west = ids[3];

      idList_t::iterator begin() {
        return ids.begin();
      }

      idList_t::iterator end() {
        return ids.end();
      }
    };

    entityId_t region;
    Matrix parentTransform;
    std::map<entityId_t, parser::pObject_t> objects;
    std::map<entityId_t, Point> objectPositions;
    std::map<entityId_t, CellDoors> cellDoors;
    bool firstPassComplete = false;

  private:
    bool constructCell(entityId_t entityId, parser::Object& obj, entityId_t parentId,
      const Matrix& parentTransform);

    bool constructCellCorner(entityId_t entityId, parser::Object& obj, entityId_t parentId,
      const Matrix& parentTransform);

    bool constructSlime(entityId_t entityId, parser::Object& obj, entityId_t parentId,
      const Matrix& parentTransform);

    bool constructCellInner(entityId_t entityId, parser::Object& obj, entityId_t parentId,
      const Matrix& parentTransform);

    bool constructCellDoor(entityId_t entityId, parser::Object& obj, entityId_t parentId,
      const Matrix& parentTransform);

    bool constructCommandScreen(entityId_t entityId, parser::Object& obj, entityId_t parentId,
      const Matrix& parentTransform);

    RootFactory& m_rootFactory;
    EntityManager& m_entityManager;
    TimeService& m_timeService;
};


}


#endif
