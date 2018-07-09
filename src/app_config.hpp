#ifndef __PROCALC_APP_CONFIG_HPP__
#define __PROCALC_APP_CONFIG_HPP__


#include <string>
#include <vector>


class AppConfig {
  public:
    typedef std::vector<std::string> CommandLineArgs;

    AppConfig(int argc, char** argv);

    void persistState();

    int stateId;
    std::string playerName;
    CommandLineArgs args;

    std::string dataPath(const std::string& relPath) const;
    std::string saveDataPath(const std::string& relPath) const;

    std::string getStringArg(unsigned int idx, const std::string& defaultVal) const;
    double getDoubleArg(unsigned int idx, double defaultVal) const;
    int getIntArg(unsigned int idx, int defaultVal) const;
};


#endif
