#include "event_system.hpp"
#include "utils.hpp"


using std::make_pair;
using std::string;
using std::list;


//===========================================
// EventSystem::listen
//===========================================
int EventSystem::listen(const string& name, handlerFunc_t fn) {
  static int nextId = 0;

  m_handlers[name].push_back(EventHandler{nextId, fn});
  return nextId++;
}

//===========================================
// EventSystem::forget
//===========================================
void EventSystem::forget(const string& name, int id) {
  m_handlers[name].remove_if([=](const EventHandler& l) { return l.id == id; });
}

//===========================================
// EventSystem::fire
//===========================================
void EventSystem::fire(const Event& event) {
  DBG_PRINT("Event: " << event.name << "\n");

  const list<EventHandler>& fns = m_handlers[event.name];
  for (auto it = fns.begin(); it != fns.end(); ++it) {
    it->handler(event);
  }
}
