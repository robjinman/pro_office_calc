#include <fstream>
#include "app_state.hpp"
#include "app_config.hpp"
#include "platform.hpp"


using std::ifstream;
using std::ofstream;


std::ostream& operator<<(std::ostream& os, const AppState& state) {
  state.serialize(os);
  return os;
}

std::istream& operator>>(std::istream& is, AppState& state) {
  state.deserialize(is);
  return is;
}
/*
//===========================================
// AppState::load
//===========================================
void AppState::load(const AppConfig& conf) {
  ifstream fin(conf.userDataDir + sep + "procalc.dat", ifstream::binary);
  fin.read(reinterpret_cast<char*>(this), sizeof(AppState));
}

//===========================================
// AppState::persist
//===========================================
void AppState::persist(const AppConfig& conf) const {
  ofstream fout(conf.userDataDir + sep + "procalc.dat", ofstream::binary);
  fout.write(reinterpret_cast<const char*>(this), sizeof(AppState));
}
*/
