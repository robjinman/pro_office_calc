#include <fstream>
#include <tinyxml2.h>
#include <QStandardPaths>
#include <QDir>
#include <QFontDatabase>
#include <QCoreApplication>
#include <QApplication>
#include "app_config.hpp"
#include "utils.hpp"


using std::ifstream;
using std::string;
using CommandLineArgs = AppConfig::CommandLineArgs;
using tinyxml2::XMLDocument;
using tinyxml2::XMLElement;


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

  readVersionFile();
  loadState();

  if (this->args.size() > 0) {
    this->stateId = getIntArg(0, 0);
  }

  QFontDatabase::addApplicationFont(dataPath("common/fonts/DejaVuSans.ttf").c_str());
  QFontDatabase::addApplicationFont(dataPath("common/fonts/DejaVuSansMono.ttf").c_str());

  this->normalFont = QFont{"DejaVu Sans"};
  this->monoFont = QFont{"DejaVu Sans Mono"};

  this->normalFont.setPixelSize(14);
  QApplication::setFont(this->normalFont);
}

//===========================================
// AppConfig::loadState
//===========================================
void AppConfig::loadState() {
  string filePath = saveDataPath("procalc.dat");

  XMLDocument doc;
  doc.LoadFile(filePath.c_str());

  if (!doc.Error()) {
    XMLElement* root = doc.FirstChildElement("config");
    XMLElement* e = root->FirstChildElement();

    while (e != nullptr) {
      string tagName(e->Name());

      m_params[tagName] = e->GetText();

      DBG_PRINT("Loaded param " << tagName << "=" << e->GetText() << "\n");

      e = e->NextSiblingElement();
    }

    this->stateId = convert<int>(GET_VALUE(m_params, "state-id"));
    m_params.erase("state-id");
  }
  else {
    DBG_PRINT("Could not load procalc.dat\n");
  }
}

//===========================================
// AppConfig::persistState
//===========================================
void AppConfig::persistState() {
  DBG_PRINT("Persisting state id " << this->stateId << "\n");

  QDir rootDir{"/"};
  rootDir.mkpath(saveDataPath("").c_str());

  string filePath = saveDataPath("procalc.dat");

  XMLDocument doc;
  XMLElement* root = doc.NewElement("config");

  doc.InsertEndChild(root);

  XMLElement* e = doc.NewElement("state-id");
  e->SetText(this->stateId);

  root->InsertEndChild(e);

  for (auto it = m_params.begin(); it != m_params.end(); ++it) {
    auto& name = it->first;
    auto& value = it->second;

    DBG_PRINT("Persisting param " << name << "=" << value << "\n");

    XMLElement* e = doc.NewElement(name.c_str());
    e->SetText(value.c_str());

    root->InsertEndChild(e);
  }

  doc.SaveFile(filePath.c_str());
}

//===========================================
// AppConfig::getParam
//===========================================
const string& AppConfig::getParam(const string& name) const {
  return GET_VALUE(m_params, name);
}

//===========================================
// AppConfig::setParam
//===========================================
void AppConfig::setParam(const string& name, const string& value) {
  m_params[name] = value;
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
// AppConfig::readVersionFile
//===========================================
void AppConfig::readVersionFile() {
  string path = versionFilePath();

  ifstream fin(path);
  if (fin.good()) {
    fin >> this->version;
  }

  fin.close();
}

//===========================================
// AppConfig::versionFilePath
//===========================================
string AppConfig::versionFilePath() const {
// Windows
#ifdef WIN32
  return QCoreApplication::applicationDirPath().toStdString() + "/VERSION";
// OS X
#elif defined(__APPLE__)
  return QCoreApplication::applicationDirPath().toStdString() + "/../Resources/VERSION";
// Linux
#else
  #ifdef DEBUG
    return QCoreApplication::applicationDirPath().toStdString() + "/VERSION";
  #else
    return string("/usr/share/procalc/VERSION");
  #endif
#endif
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
