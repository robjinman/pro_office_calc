#include "event_system.hpp"
#include "utils.hpp"


using std::string;
using std::map;
using std::vector;


//===========================================
// EventSystem::listen
//===========================================
int EventSystem::listen(const string& name, handlerFunc_t fn) {
  static int nextId = 0;

  m_handlers[name][nextId] = fn;

  return nextId++;
}

//===========================================
// EventSystem::forget
//===========================================
void EventSystem::forget(int id) {

  for (auto it = m_handlers.begin(); it != m_handlers.end(); ++it) {
    map<int, handlerFunc_t>& fns = it->second;
    fns.erase(id);
  }
}

//===========================================
// EventSystem::fire
//===========================================
void EventSystem::fire(const string& name, const Event& event) {
  DBG_PRINT("Calling handlers for: " << name << "\n");

  auto it = m_handlers.find(name);
  if (it != m_handlers.end()) {
    const map<int, handlerFunc_t>& fns = it->second;

    for (auto jt = fns.begin(); jt != fns.end(); ++jt) {
      jt->second(event);
    }
  }
}

//===========================================
// EventSystem::fire
//===========================================
void EventSystem::fire(const Event& event) {
  DBG_PRINT("Event: " << event.name << "\n");

  vector<string> v = splitString(event.name, '.');
  string name;

  fire(name, event);

  for (auto it = v.begin(); it != v.end(); ++it) {
    if (name.length() > 0) {
      name.append(".");
    }
    name.append(*it);

    fire(name, event);
  }
}
