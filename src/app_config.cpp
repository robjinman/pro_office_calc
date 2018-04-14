#include <QStandardPaths>
#include "app_config.hpp"


using std::string;


namespace config {


string dataPath(const string& relPath) {
#ifdef DEBUG
  #ifdef WIN32
    return string("./data/") + relPath;
  #else
    return string("./usr/share/procalc/data/") + relPath;
  #endif
#else
  return QStandardPaths::locate(QStandardPaths::AppDataLocation, relPath.c_str()).toStdString();
#endif
}


}
