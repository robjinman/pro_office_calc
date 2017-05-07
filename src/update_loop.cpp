#include <QTimer>
#include "update_loop.hpp"


using std::weak_ptr;
using std::unique_ptr;


//===========================================
// UpdateLoop::UpdateLoop
//===========================================
UpdateLoop::UpdateLoop(unique_ptr<QTimer> timer, int interval)
  : m_timer(std::move(timer)),
    m_interval(interval) {}

//===========================================
// UpdateLoop::addObject
//===========================================
void UpdateLoop::addObject(weak_ptr<IUpdatable> obj) {
  m_objects.push_back(obj);

  if (!m_timer->isActive()) {
    m_timer->start(m_interval);
  }
}

//===========================================
// UpdateLoop::numObjects
//===========================================
int UpdateLoop::numObjects() const {
  return m_objects.size();
}

//===========================================
// UpdateLoop::update
//===========================================
void UpdateLoop::update() {
  auto it = m_objects.begin();

  while (it != m_objects.end()) {
    auto pObj = it->lock();

    if (!pObj || !pObj->update()) {
      m_objects.erase(it++);
    }
    else {
      ++it;
    }
  }
}
