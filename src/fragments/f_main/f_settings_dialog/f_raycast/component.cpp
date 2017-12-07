#include <set>
#include "fragments/f_main/f_settings_dialog/f_raycast/component.hpp"


using std::set;
using std::string;


static set<entityId_t> reserved;
entityId_t Component::nextId = 0;


//===========================================
// hash
//===========================================
static entityId_t hash(const char* str) {
  entityId_t hash = 5381;
  int c;

  while ((c = *str++)) {
    hash = ((hash << 5) + hash) + c;
  }

  return hash;
}

//===========================================
// Component::getNextId
//===========================================
entityId_t Component::getNextId() {
  entityId_t id = nextId++;
  while (reserved.count(id) == 1) {
    id = nextId++;
  }
  return id;
}

//===========================================
// Component::getIdFromString
//===========================================
entityId_t Component::getIdFromString(const string& s) {
  entityId_t id = hash(s.c_str());
  reserved.insert(id);

  return id;
}
