#include <QStandardPaths>
#include "app_config.hpp"


using std::string;


namespace config {


string dataPath(const string& relPath) {
#ifdef DEBUG
  return string("./usr/local/share/procalc/data/") + relPath;
#else
  return QStandardPaths::locate(QStandardPaths::AppDataLocation, relPath.c_str()).toStdString();
#endif
}


}
