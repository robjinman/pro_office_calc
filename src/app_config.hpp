#ifndef __PROCALC_APP_CONFIG_HPP__
#define __PROCALC_APP_CONFIG_HPP__


#include <string>
#include <vector>


namespace config {


std::string dataPath(const std::string& relPath);
std::string saveDataPath(const std::string& relPath);

typedef std::vector<std::string> CommandLineArgs;

CommandLineArgs getArgs(int argc, char** argv);

std::string getStringArg(const CommandLineArgs& args, int idx, std::string defaultVal);
double getDoubleArg(const CommandLineArgs& args, int idx, double defaultVal);
int getIntArg(const CommandLineArgs& args, int idx, int defaultVal);


}


#endif
