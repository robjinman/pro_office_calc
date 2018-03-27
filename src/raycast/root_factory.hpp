#ifndef __PROCALC_RAYCAST_ROOT_FACTORY_HPP__
#define __PROCALC_RAYCAST_ROOT_FACTORY_HPP__


#include <map>
#include <string>
#include <vector>
#include "raycast/game_object_factory.hpp"


class RootFactory : public GameObjectFactory {
  public:
    RootFactory();

    const std::set<std::string>& types() const override;

    void addFactory(pGameObjectFactory_t factory);

    bool constructObject(const std::string& type, entityId_t entityId, parser::Object& obj,
      entityId_t parentId, const Matrix& parentTransform) override;

  private:
    std::vector<pGameObjectFactory_t> m_factories;
    std::map<std::string, GameObjectFactory*> m_factoriesByType;

    std::set<std::string> m_types;

#ifdef DEBUG
    int m_dbgIndentLvl = 0;
#endif
};


#endif
