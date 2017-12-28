#include <cassert>
#include <QEvent>
#include <QCoreApplication>
#include "event_system.hpp"
#include "utils.hpp"


using std::string;
using std::map;
using std::vector;


static int CUSTOM_EVENT_TYPE = QEvent::registerEventType();

struct CustomEvent : public QEvent {
  CustomEvent(pEvent_t event)
    : QEvent(static_cast<QEvent::Type>(CUSTOM_EVENT_TYPE)),
      event(std::move(event)) {}

  pEvent_t event;
};


//===========================================
// EventSystem::event
//===========================================
bool EventSystem::event(QEvent* e) {
  if (e->type() == CUSTOM_EVENT_TYPE) {
    CustomEvent* event = dynamic_cast<CustomEvent*>(e);
    assert(event != nullptr);

    processEvent(*event->event);

    return true;
  }
  return QObject::event(e);
}

//===========================================
// EventSystem::listen
//===========================================
int EventSystem::listen(const string& name, handlerFunc_t fn) {
  static int nextId = 0;

  if (m_processingEvent) {
    m_pendingAddition[name][nextId] = fn;
  }
  else {
    m_handlers[name][nextId] = fn;
  }

  return nextId++;
}

//===========================================
// EventSystem::forget_
//===========================================
void EventSystem::forget_(int id) {
  for (auto it = m_handlers.begin(); it != m_handlers.end(); ++it) {
    map<int, handlerFunc_t>& fns = it->second;
    fns.erase(id);
  }
}

//===========================================
// EventSystem::forget
//===========================================
void EventSystem::forget(int id) {
  if (m_processingEvent) {
    m_pendingForget.insert(id);
  }
  else {
    forget_(id);
  }
}

//===========================================
// EventSystem::addPending
//===========================================
void EventSystem::addPending() {
  for (auto it = m_pendingAddition.begin(); it != m_pendingAddition.end(); ++it) {
    string name = it->first;
    auto handlersById = it->second;

    for (auto jt = handlersById.begin(); jt != handlersById.end(); ++jt) {
      int id = jt->first;
      auto handlerFn = jt->second;

      m_handlers[name][id] = handlerFn;
    }
  }

  m_pendingAddition.clear();
}

//===========================================
// EventSystem::forgetPending
//===========================================
void EventSystem::forgetPending() {
  for (auto it = m_pendingForget.begin(); it != m_pendingForget.end(); ++it) {
    forget_(*it);
  }

  m_pendingForget.clear();
}

//===========================================
// EventSystem::processingStart
//===========================================
void EventSystem::processingStart() {
  m_processingEvent = true;
}

//===========================================
// EventSystem::processingEnd
//===========================================
void EventSystem::processingEnd() {
  forgetPending();
  addPending();

  m_processingEvent = false;
}

//===========================================
// EventSystem::processEvent_
//===========================================
void EventSystem::processEvent_(const string& name, const Event& event) {
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
// EventSystem::processEvent
//===========================================
void EventSystem::processEvent(const Event& event) {
  processingStart();

  vector<string> v = splitString(event.name, '.');
  string name;

  processEvent_(name, event);

  for (auto it = v.begin(); it != v.end(); ++it) {
    if (name.length() > 0) {
      name.append(".");
    }
    name.append(*it);

    processEvent_(name, event);
  }

  processingEnd();
}

//===========================================
// EventSystem::fire
//===========================================
void EventSystem::fire(pEvent_t event) {
  DBG_PRINT("Event: " << event->name << "\n");
  QCoreApplication::postEvent(this, new CustomEvent(std::move(event)));
}
