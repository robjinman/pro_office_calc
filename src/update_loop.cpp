#include "update_loop.hpp"


using std::weak_ptr;
using std::function;


//===========================================
// UpdateLoop::UpdateLoop
//===========================================
UpdateLoop::UpdateLoop(int interval)
  : m_interval(interval) {

  m_timer = makeQtObjPtr<QTimer>();
  connect(m_timer.get(), SIGNAL(timeout()), this, SLOT(tick()));
}

//===========================================
// UpdateLoop::addFunction
//===========================================
void UpdateLoop::add(function<bool()> fn, function<void()> fnOnFinish) {
  m_functions.push_back(FuncPair{fn, fnOnFinish});

  if (!m_timer->isActive()) {
    m_timer->start(m_interval);
  }
}

//===========================================
// UpdateLoop::finishAll
//===========================================
void UpdateLoop::finishAll() {
  m_timer->stop();
}

//===========================================
// UpdateLoop::size
//===========================================
int UpdateLoop::size() const {
  return static_cast<int>(m_functions.size());
}

//===========================================
// UpdateLoop::fps
//===========================================
double UpdateLoop::fps() const {
  return 1000.0 / static_cast<double>(m_interval);
}

//===========================================
// UpdateLoop::tick
//===========================================
void UpdateLoop::tick() {
  auto it = m_functions.begin();

  while (it != m_functions.end()) {
    bool result = false;

    if (m_timer->isActive()) {
      result = it->fnPeriodic();
    }

    if (!result) {
      it->fnFinish();
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
