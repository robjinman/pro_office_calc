#ifndef __PROCALC_FRAGMENTS_F_RAYCAST_TIMING_HPP_
#define __PROCALC_FRAGMENTS_F_RAYCAST_TIMING_HPP_


#include <functional>
#include <random>


class Debouncer {
  public:
    Debouncer(double seconds);

    bool ready();

  private:
    double m_duration;
    double m_start;
};

class TimePattern {
  public:
    virtual bool doIfReady(std::function<void()> fn) = 0;

    virtual ~TimePattern() {}
};

class TRandomIntervals : public TimePattern {
  public:
    TRandomIntervals(unsigned long min, unsigned long max);

    virtual bool doIfReady(std::function<void()> fn) override;

    virtual ~TRandomIntervals() override {}

  private:
    std::mt19937 m_randEngine;
    std::uniform_real_distribution<> m_distribution;
    unsigned long m_dueTime;

    void calcDueTime();
};


#endif
