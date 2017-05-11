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
// UpdateLoop::addFunction
//===========================================
void UpdateLoop::add(std::function<bool()> fn) {
  m_functions.push_back(fn);

  if (!m_timer->isActive()) {
    m_timer->start(m_interval);
  }
}

//===========================================
// UpdateLoop::size
//===========================================
int UpdateLoop::size() const {
  return m_functions.size();
}

//===========================================
// UpdateLoop::fps
//===========================================
double UpdateLoop::fps() const {
  return 1000.0 / static_cast<double>(m_interval);
}

//===========================================
// UpdateLoop::update
//===========================================
void UpdateLoop::update() {
  auto it = m_functions.begin();

  while (it != m_functions.end()) {
    if (!(*it)()) {
      m_functions.erase(it++);
    }
    else {
      ++it;
    }
  }

  if (m_functions.empty()) {
    m_timer->stop();
  }
}
