#ifndef __PROCALC_RAYCAST_TWEEN_CURVES_HPP__
#define __PROCALC_RAYCAST_TWEEN_CURVES_HPP__


#include <functional>


std::function<double(int)> cubicOut(double from, double to, int n) {
  return [=](int i) {
    return (to - from) * (pow(static_cast<double>(i) / n - 1, 3) + 1) + from;
  };
}


#endif
