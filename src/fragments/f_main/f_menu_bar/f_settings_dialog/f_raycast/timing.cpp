#include <chrono>
#include "fragments/f_main/f_menu_bar/f_settings_dialog/f_raycast/timing.hpp"


namespace chrono = std::chrono;
using std::function;


//===========================================
// getTime
//===========================================
inline static unsigned long getTime() {
  return chrono::high_resolution_clock::now().time_since_epoch() / chrono::milliseconds(1);
}

//===========================================
// now
//===========================================
inline static double now() {
  return 0.001 * getTime();
}

//===========================================
// Debouncer::Debouncer
//===========================================
Debouncer::Debouncer(double seconds)
  : m_duration(seconds),
    m_start(now()) {}

//===========================================
// Debouncer::ready
//===========================================
bool Debouncer::ready() {
  double t = now();
  if (t - m_start >= m_duration) {
    m_start = t;
    return true;
  }
  return false;
}

//===========================================
// Debouncer::reset
//===========================================
void Debouncer::reset() {
  m_start = now();
}

//===========================================
// TRandomIntervals::TRandomIntervals
//===========================================
TRandomIntervals::TRandomIntervals(unsigned long min, unsigned long max) {
  std::random_device rd;

  m_randEngine.seed(rd());
  m_distribution = std::uniform_real_distribution<>(min, max);

  calcDueTime();
}

//===========================================
// TRandomIntervals::doIfReady
//===========================================
bool TRandomIntervals::doIfReady(function<void()> fn) {
  double t = getTime();

  if (t >= m_dueTime) {
    fn();
    calcDueTime();

    return true;
  }

  return false;
}

//===========================================
// TRandomIntervals::calcDueTime
//===========================================
void TRandomIntervals::calcDueTime() {
  m_dueTime = getTime() + m_distribution(m_randEngine);
}
