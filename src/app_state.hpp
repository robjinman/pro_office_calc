#ifndef __PROCALC_APP_STATE_HPP__
#define __PROCALC_APP_STATE_HPP__


struct AppState {
  virtual void setup(int rootState) = 0;
  virtual void ~AppState() {}
};


#endif
