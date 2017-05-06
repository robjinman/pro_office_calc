#ifndef __PROCALC_APP_STATE_HPP__
#define __PROCALC_APP_STATE_HPP__


class AppConfig;

struct AppState {
  //uint8_t count = 10;

  void load(const AppConfig& conf);
  void persist(const AppConfig& conf) const;
};


#endif
