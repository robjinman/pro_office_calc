#ifndef __PROCALC_APP_CONFIG_HPP__
#define __PROCALC_APP_CONFIG_HPP__


#include <string>
#include <vector>


namespace config {


std::string dataPath(const std::string& relPath);
std::string saveDataPath(const std::string& relPath);

typedef std::vector<std::string> CommandLineArgs;

CommandLineArgs getArgs(int argc, char** argv);

std::string getStringArg(const CommandLineArgs& args, unsigned int idx, std::string defaultVal);
double getDoubleArg(const CommandLineArgs& args, unsigned int idx, double defaultVal);
int getIntArg(const CommandLineArgs& args, unsigned int idx, int defaultVal);


}


#endif
