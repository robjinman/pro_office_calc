#ifndef __PROCALC_RAYCAST_TIME_SERVICE_HPP__
#define __PROCALC_RAYCAST_TIME_SERVICE_HPP__


#include <functional>
#include <map>
#include <set>
#include <string>


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
    long onTimeout(std::function<void()> fn, double seconds);
    long atIntervals(std::function<bool()> fn, double seconds);
    void cancelTimeout(long id);
    void update();

    double now() const;

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

    struct Interval {
      std::function<bool()> fn;
      double duration;
      long long start;
    };

    long long m_frame = 0;
    std::map<std::string, TweenWrap> m_tweens;
    std::map<long, Timeout> m_timeouts;
    std::map<long, Interval> m_intervals;
    std::set<long> m_pendingDeletion;

    void deletePending();
};


#endif
