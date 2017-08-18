#ifndef __PROCALC_FRAGMENTS_F_RAYCAST_TIME_SERVICE_HPP__
#define __PROCALC_FRAGMENTS_F_RAYCAST_TIME_SERVICE_HPP__


#include <functional>
#include <map>
#include <string>


struct Tween {
  std::function<bool()> tick;
  std::function<void()> finish;
};

class TimeService {
  public:
    void addTween(const Tween& tween, const char* name = nullptr);
    void update();

  private:
    std::map<std::string, Tween> m_tweens;
};


#endif
