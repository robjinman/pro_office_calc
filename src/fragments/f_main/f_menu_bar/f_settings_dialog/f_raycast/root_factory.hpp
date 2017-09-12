#ifndef __PROCALC_FRAGMENTS_F_RAYCAST_ROOT_FACTORY_HPP__
#define __PROCALC_FRAGMENTS_F_RAYCAST_ROOT_FACTORY_HPP__


#include <map>
#include <string>
#include <vector>
#include "fragments/f_main/f_menu_bar/f_settings_dialog/f_raycast/game_object_factory.hpp"


class EntityManager;
class AudioService;
class TimeService;

class RootFactory : public GameObjectFactory {
  public:
    RootFactory(EntityManager& entityManager, AudioService& audioService, TimeService& timeService);

    virtual const std::set<std::string>& types() const override;

    virtual bool constructObject(const std::string& type, entityId_t entityId,
      const parser::Object& obj, entityId_t parentId, const Matrix& parentTransform) override;

    virtual ~RootFactory() override {}

  private:
    std::vector<pGameObjectFactory_t> m_factories;
    std::map<std::string, GameObjectFactory*> m_factoriesByType;

    std::set<std::string> m_types;

#ifdef DEBUG
    int m_dbgIndentLvl = 0;
#endif
};


#endif
