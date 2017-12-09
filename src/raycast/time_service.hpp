#ifndef __PROCALC_RAYCAST_TIME_SERVICE_HPP__
#define __PROCALC_RAYCAST_TIME_SERVICE_HPP__


#include <functional>
#include <map>
#include <string>
#include <list>


struct Tween {
  // Args: frame, elapsed, frameRate
  std::function<bool(long, double, double)> tick;
  std::function<void(long, double, double)> finish;
};

class TimeService {
  public:
    TimeService(double frameRate)
      : frameRate(frameRate) {}

    void addTween(const Tween& tween, const char* name = nullptr);
    void onTimeout(std::function<void()> fn, double seconds);
    void update();

    const double frameRate;

  private:
    struct TweenWrap {
      Tween tween;
      long long start;
    };

    struct Timeout {
      std::function<void()> fn;
      double duration;
      long long start;
    };

    long long m_frame = 0;
    std::map<std::string, TweenWrap> m_tweens;
    std::list<Timeout> m_timeouts;
};


#endif
