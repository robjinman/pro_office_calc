#include <fstream>
#include <QStandardPaths>
#include <QCoreApplication>
#include "app_config.hpp"
#include "utils.hpp"


using std::string;
using std::ifstream;
using std::ofstream;
using CommandLineArgs = AppConfig::CommandLineArgs;


//===========================================
// convert
//===========================================
template<typename T>
T convert(const string&) {
  T t;
  return t;
}

template<>
double convert<double>(const string& s) {
  return std::stod(s);
}

template<>
int convert<int>(const string& s) {
  return std::stoi(s);
}
//===========================================

//===========================================
// getArg
//===========================================
template<typename T>
T getArg(const CommandLineArgs& args, unsigned int idx, const T& defaultVal) {
  if (idx < args.size()) {
    return convert<T>(args[idx]);
  }
  else {
    return defaultVal;
  }
}

//===========================================
// AppConfig::AppConfig
//===========================================
AppConfig::AppConfig(int argc, char** argv) {
  for (int i = 1; i < argc; ++i) {
    this->args.push_back(argv[i]);
  }

  if (this->args.size() > 0) {
    this->stateId = getIntArg(0, 0);
  }
  else {
    ifstream fin(saveDataPath("procalc.dat"), ifstream::binary);

    if (fin.good()) {
      fin.read(reinterpret_cast<char*>(&this->stateId), sizeof(this->stateId));
    }
  }
}

//===========================================
// AppConfig::getDoubleArg
//===========================================
double AppConfig::getDoubleArg(unsigned int idx, double defaultVal) const {
  return getArg<double>(this->args, idx, defaultVal);
}

//===========================================
// AppConfig::getIntArg
//===========================================
int AppConfig::getIntArg( unsigned int idx, int defaultVal) const {
  return getArg<int>(this->args, idx, defaultVal);
}

//===========================================
// AppConfig::getStringArg
//===========================================
string AppConfig::getStringArg(unsigned int idx, const string& defaultVal) const {
  return getArg<string>(this->args, idx, defaultVal);
}

//===========================================
// AppConfig::persistState
//===========================================
void AppConfig::persistState() {
  DBG_PRINT("Persisting state id " << this->stateId << "\n");

  ofstream fout(saveDataPath("procalc.dat"), ofstream::binary | ofstream::trunc);
  fout.write(reinterpret_cast<const char*>(&this->stateId), sizeof(this->stateId));
}

//===========================================
// AppConfig::dataPath
//===========================================
string AppConfig::dataPath(const string& relPath) const {
// Windows
#ifdef WIN32
  return QCoreApplication::applicationDirPath().toStdString() + "/data/" + relPath;
// OS X
#elif defined(__APPLE__)
  return QCoreApplication::applicationDirPath().toStdString() + "/../Resources/data/" + relPath;
// Linux
#else
  #ifdef DEBUG
    return QCoreApplication::applicationDirPath().toStdString() + "/data/" + relPath;
  #else
    return string("/usr/share/procalc/data/") + relPath;
  #endif
#endif
}

//===========================================
// AppConfig::saveDataPath
//===========================================
string AppConfig::saveDataPath(const string& relPath) const {
#ifdef DEBUG
  return QCoreApplication::applicationDirPath().toStdString() + "/" + relPath;
#else
  static string dir;

  if (dir.length() == 0) {
    auto paths = QStandardPaths::standardLocations(QStandardPaths::AppDataLocation);
    if (!paths.empty()) {
      dir = paths.first().toStdString();
    }
  }

  return dir + "/" + relPath;
#endif
}
