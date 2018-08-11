#include <sstream>
#include "raycast/time_service.hpp"


using std::string;
using std::stringstream;
using std::function;


static long nextId = 0;


//===========================================
// TimeService::now
//===========================================
double TimeService::now() const {
  return m_frame / frameRate;
}

//===========================================
// TimeService::addTween
//===========================================
void TimeService::addTween(const Tween& tween, string name) {
  if (name.empty()) {
    stringstream ss;
    ss << "tween" << rand();
    name = ss.str();
  }

  if (m_tweens.find(name) == m_tweens.end()) {
    m_tweens[name] = TweenWrap{tween, m_frame};
  }
}

//===========================================
// TimeService::removeTween
//===========================================
void TimeService::removeTween(const string& name) {
  m_tweens.erase(name);
}

//===========================================
// TimeService::deletePending
//===========================================
void TimeService::deletePending() {
  for (long id : m_pendingDeletion) {
    m_timeouts.erase(id);
    m_intervals.erase(id);
  }

  m_pendingDeletion.clear();
}

//===========================================
// TimeService::update
//===========================================
void TimeService::update() {
  deletePending();

  double elapsed = m_frame / frameRate;

  for (auto it = m_tweens.begin(); it != m_tweens.end();) {
    const TweenWrap& tweenWrap = it->second;
    const Tween& tween = tweenWrap.tween;
    long i = static_cast<long>(m_frame - tweenWrap.start);

    if (!tween.tick(i, elapsed, frameRate)) {
      tween.finish(i, elapsed, frameRate);
      m_tweens.erase(it++);
    }
    else {
      ++it;
    }
  }

  for (auto it = m_timeouts.begin(); it != m_timeouts.end();) {
    const Timeout& timeout = it->second;

    long frames = static_cast<long>(m_frame - timeout.start);
    double elapsed = frames / frameRate;

    if (elapsed >= timeout.duration) {
      timeout.fn();

      it = m_timeouts.erase(it);
    }
    else {
      ++it;
    }
  }

  for (auto it = m_intervals.begin(); it != m_intervals.end();) {
    Interval& interval = it->second;

    long frames = static_cast<long>(m_frame - interval.start);
    double elapsed = frames / frameRate;

    if (elapsed >= interval.duration) {
      bool cont = interval.fn();
      interval.start = m_frame;

      if (!cont) {
        it = m_intervals.erase(it);
      }
    }
    else {
      ++it;
    }
  }

  ++m_frame;
}

//===========================================
// TimeService::onTimeout
//===========================================
long TimeService::onTimeout(function<void()> fn, double seconds) {
  m_timeouts[nextId] = Timeout{fn, seconds, m_frame};
  return nextId++;
}

//===========================================
// TimeService::atIntervals
//
// fn should return true to continue the interval, false to cancel
//===========================================
long TimeService::atIntervals(function<bool()> fn, double seconds) {
  m_intervals[nextId] = Interval{fn, seconds, m_frame};
  return nextId++;
}

//===========================================
// TimeService::cancelTimeout
//===========================================
void TimeService::cancelTimeout(long id) {
  m_pendingDeletion.insert(id);
}
