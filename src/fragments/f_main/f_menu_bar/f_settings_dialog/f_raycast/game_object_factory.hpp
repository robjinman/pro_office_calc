#ifndef __PROCALC_FRAGMENTS_F_RAYCAST_GAME_OBJECT_FACTORY_HPP__
#define __PROCALC_FRAGMENTS_F_RAYCAST_GAME_OBJECT_FACTORY_HPP__


#include <string>
#include <memory>
#include <set>
#include "fragments/f_main/f_menu_bar/f_settings_dialog/f_raycast/component.hpp"


class EntityManager;
class AudioService;
class TimeService;
class Matrix;
namespace parser { class Object; }

class GameObjectFactory {
  public:
    virtual bool constructObject(const std::string& type, entityId_t entityId,
      const parser::Object& obj, entityId_t parentId, const Matrix& parentTransform) = 0;

    virtual const std::set<std::string>& types() const = 0;

    virtual ~GameObjectFactory() {}
};

typedef std::unique_ptr<GameObjectFactory> pGameObjectFactory_t;


#endif
