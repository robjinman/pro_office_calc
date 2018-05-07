#include <QStandardPaths>
#include <QCoreApplication>
#include "app_config.hpp"


using std::string;


namespace config {


//===========================================
// convert
//===========================================
template<typename T>
T convert(const string&) {}

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
T getArg(const CommandLineArgs& args, int idx, const T& defaultVal) {
  if (idx < args.size()) {
    return convert<T>(args[idx]);
  }
  else {
    return defaultVal;
  }
}

//===========================================
// get*Arg
//===========================================
double getDoubleArg(const CommandLineArgs& args, int idx, double defaultVal) {
  return getArg<double>(args, idx, defaultVal);
}

int getIntArg(const CommandLineArgs& args, int idx, int defaultVal) {
  return getArg<int>(args, idx, defaultVal);
}

string getStringArg(const CommandLineArgs& args, int idx, const string& defaultVal) {
  return getArg<string>(args, idx, defaultVal);
}
//===========================================

//===========================================
// getArgs
//===========================================
CommandLineArgs getArgs(int argc, char** argv) {
  CommandLineArgs args;

  for (int i = 1; i < argc; ++i) {
    args.push_back(argv[i]);
  }

  return args;
}

//===========================================
// dataPath
//===========================================
string dataPath(const string& relPath) {
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
// saveDataPath
//===========================================
string saveDataPath(const string& relPath) {
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


}
