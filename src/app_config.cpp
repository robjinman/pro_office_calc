#include <QStandardPaths>
#include "app_config.hpp"


using std::string;


namespace config {


string dataPath(const string& relPath) {
#ifdef WIN32
  return string("./data/") + relPath;
#else
  #ifdef DEBUG
    return string("./data/") + relPath;
  #else
    return string("/usr/share/procalc/data/") + relPath;
  #endif
#endif
}

string saveDataPath(const string& relPath) {
#ifdef DEBUG
  return string("./data/") + relPath;
#else
  static string dir;

  if (dir.length() == 0) {
    auto& paths = QStandardPaths::standardLocations(QStandardPaths::AppDataLocation);
    if (!paths.empty()) {
      dir = paths.first().toStdString();
    }
  }

  return dir + "/" + relPath;
#endif
}


}
