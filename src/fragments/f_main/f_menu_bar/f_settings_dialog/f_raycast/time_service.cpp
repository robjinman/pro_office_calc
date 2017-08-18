#include <sstream>
#include "fragments/f_main/f_menu_bar/f_settings_dialog/f_raycast/time_service.hpp"


using std::string;
using std::stringstream;


//===========================================
// TimeService::addTween
//===========================================
void TimeService::addTween(const Tween& tween, const char* name) {
  string s;
  if (name != nullptr) {
    s.assign(name);
  }
  else {
    stringstream ss;
    ss << "tween" << rand();
    s = ss.str();
  }

  if (m_tweens.find(s) == m_tweens.end()) {
    m_tweens[s] = tween;
  }
}

//===========================================
// TimeService::update
//===========================================
void TimeService::update() {
  for (auto it = m_tweens.begin(); it != m_tweens.end();) {
    const Tween& tween = it->second;

    if (!tween.tick()) {
      tween.finish();
      m_tweens.erase(it++);
    }
    else {
      ++it;
    }
  }
}
