#include <QStandardPaths>
#include <QCoreApplication>
#include "app_config.hpp"


using std::string;


namespace config {


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
