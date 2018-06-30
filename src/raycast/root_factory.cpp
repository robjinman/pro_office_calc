#include <algorithm>
#include "raycast/root_factory.hpp"
#include "raycast/map_parser.hpp"
#include "utils.hpp"


using std::string;
using std::set;


//===========================================
// RootFactory::RootFactory
//===========================================
RootFactory::RootFactory() {}

//===========================================
// RootFactory::addFactory
//===========================================
void RootFactory::addFactory(pGameObjectFactory_t factory) {
  const set<string>& types = factory->types();
  for (auto jt = types.begin(); jt != types.end(); ++jt) {
    m_factoriesByType[*jt] = factory.get();
    m_types.insert(*jt);
  }

  m_factories.push_back(std::move(factory));
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
bool RootFactory::constructObject(const string& type, entityId_t entityId, parser::Object& obj,
  entityId_t parentId, const Matrix& parentTransform) {

#ifdef DEBUG
  const string& name = getValue(obj.dict, "name", "");

  //DBG_PRINT(string(m_dbgIndentLvl++, '\t') << "Constructing " << type
  //  << (name.length() > 0 ? string(" (") + name + ")" : "") << "...\n");
#endif

  auto it = m_factoriesByType.find(type);
  if (it != m_factoriesByType.end()) {
    bool success = it->second->constructObject(type, entityId, obj, parentId, parentTransform);

    //DBG_PRINT(string(--m_dbgIndentLvl, '\t') << (success ? "Success\n" : "Fail\n"));
    return success;
  }
  else {
    //DBG_PRINT("No factory knows how to make object of type '" << type << "'\n");
  }

  return false;
}
