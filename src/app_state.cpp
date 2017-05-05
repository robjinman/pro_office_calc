#include <fstream>
#include "app_state.hpp"
#include "app_config.hpp"
#include "platform.hpp"


using std::ifstream;
using std::ofstream;


void AppState::load(const AppConfig& conf) {
  ifstream fin(conf.userDataDir + sep + "procalc.dat", ifstream::binary);
  fin.read(reinterpret_cast<char*>(this), sizeof(AppState));
}

void AppState::persist(const AppConfig& conf) const {
  ofstream fout(conf.userDataDir + sep + "procalc.dat", ofstream::binary);
  fout.write(reinterpret_cast<const char*>(this), sizeof(AppState));
}
