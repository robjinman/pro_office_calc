#include <algorithm>
#include "fragments/f_main/f_settings_dialog/f_raycast/root_factory.hpp"
#include "fragments/f_main/f_settings_dialog/f_raycast/misc_factory.hpp"
#include "fragments/f_main/f_settings_dialog/f_raycast/sprite_factory.hpp"
#include "fragments/f_main/f_settings_dialog/f_raycast/geometry_factory.hpp"
#include "utils.hpp"


using std::string;
using std::set;


//===========================================
// RootFactory::RootFactory
//===========================================
RootFactory::RootFactory(EntityManager& entityManager, AudioService& audioService,
  TimeService& timeService) {

  m_factories.push_back(pGameObjectFactory_t(new MiscFactory(*this, entityManager, audioService,
    timeService)));
  m_factories.push_back(pGameObjectFactory_t(new SpriteFactory(*this, entityManager, audioService,
    timeService)));
  m_factories.push_back(pGameObjectFactory_t(new GeometryFactory(*this, entityManager, audioService,
    timeService)));

  for (auto it = m_factories.begin(); it != m_factories.end(); ++it) {
    const GameObjectFactory& factory = **it;

    const set<string>& types = factory.types();
    for (auto jt = types.begin(); jt != types.end(); ++jt) {
      m_factoriesByType[*jt] = it->get();
      m_types.insert(*jt);
    }
  }
}

//===========================================
// RootFactory::types
//===========================================
const set<string>& RootFactory::types() const {
  return m_types;
}

//===========================================
// RootFactory::constructObject
//===========================================
bool RootFactory::constructObject(const string& type, entityId_t entityId,
  const parser::Object& obj, entityId_t parentId, const Matrix& parentTransform) {

  DBG_PRINT(string(m_dbgIndentLvl++, '\t') << "Constructing " << type << "...\n");

  auto it = m_factoriesByType.find(type);
  if (it != m_factoriesByType.end()) {
    bool success = it->second->constructObject(type, entityId, obj, parentId, parentTransform);

    DBG_PRINT(string(--m_dbgIndentLvl, '\t') << (success ? "Success\n" : "Fail\n"));
    return success;
  }
  else {
    DBG_PRINT("No factory knows how to make object of type '" << type << "'\n");
  }

  return false;
}
