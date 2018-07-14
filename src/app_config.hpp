#ifndef __PROCALC_APP_CONFIG_HPP__
#define __PROCALC_APP_CONFIG_HPP__


#include <string>
#include <map>
#include <vector>
#include <QFont>
#include "event.hpp"


struct RequestStateChangeEvent : public Event {
  RequestStateChangeEvent(int stateId, bool hardReset = false)
    : Event("requestStateChange"),
      stateId(stateId),
      hardReset(hardReset) {}

    int stateId;
    bool hardReset;
};

struct SetConfigParamEvent : public Event {
  SetConfigParamEvent(const std::string& name, const std::string& value)
    : Event("setConfigParam"),
      name(name),
      value(value) {}

  std::string name;
  std::string value;
};

class AppConfig {
  public:
    typedef std::vector<std::string> CommandLineArgs;

    AppConfig(int argc, char** argv);

    void persistState();

    int stateId = 0;
    CommandLineArgs args;

    QFont normalFont;
    QFont monoFont;

    const std::string& getParam(const std::string& name) const;
    void setParam(const std::string& name, const std::string& value);

    std::string dataPath(const std::string& relPath) const;
    std::string saveDataPath(const std::string& relPath) const;

    std::string getStringArg(unsigned int idx, const std::string& defaultVal) const;
    double getDoubleArg(unsigned int idx, double defaultVal) const;
    int getIntArg(unsigned int idx, int defaultVal) const;

  private:
    void loadState();

    std::map<std::string, std::string> m_params;
};


#endif
