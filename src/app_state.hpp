#ifndef __PROCALC_APP_STATE_HPP__
#define __PROCALC_APP_STATE_HPP__


#include <memory>


struct AppState {
  virtual void initialise(int rootState) = 0;
  virtual ~AppState() {}
};

typedef std::unique_ptr<AppState> pAppState_t;


#endif
