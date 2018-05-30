#include "fragments/f_main/f_app_dialog/f_file_system/object_factory.hpp"


using std::set;
using std::string;


namespace going_in_circles {


//===========================================
// ObjectFactory::ObjectFactory
//===========================================
ObjectFactory::ObjectFactory(RootFactory& rootFactory, EntityManager& entityManager,
  TimeService& timeService)
  : m_rootFactory(rootFactory),
    m_entityManager(entityManager),
    m_timeService(timeService) {}

//===========================================
// ObjectFactory::types
//===========================================
const set<string>& ObjectFactory::types() const {
  static const set<string> types{};
  return types;
}

//===========================================
// ObjectFactory::constructObject
//===========================================
bool ObjectFactory::constructObject(const string& type, entityId_t entityId,
  parser::Object& obj, entityId_t region, const Matrix& parentTransform) {

  // TODO

  return false;
}


}
