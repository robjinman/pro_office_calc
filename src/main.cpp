#include <iostream>
#include <fstream>
#include <memory>
#ifdef __APPLE__
#  include "CoreFoundation/CoreFoundation.h"
#endif
#include "application.hpp"
#include "exception.hpp"
#include "app_config.hpp"
#include "root_spec_factory.hpp"
#include "event_system.hpp"
#include "platform.hpp"
#include "utils.hpp"
#include "request_state_change_event.hpp"
#include "fragments/main_fragment.hpp"


#ifdef __APPLE__

//===========================================
// changeWorkingDir
//===========================================
void changeWorkingDir() {
  CFBundleRef mainBundle = CFBundleGetMainBundle();
  CFURLRef resourcesURL = CFBundleCopyResourcesDirectoryURL(mainBundle);

  char path[PATH_MAX];
  CFURLGetFileSystemRepresentation(resourcesURL, TRUE, reinterpret_cast<UInt8*>(path), PATH_MAX);

  CFRelease(resourcesURL);
  chdir(path);
}

#endif


using std::ifstream;
using std::ofstream;
using std::cout;
using std::cerr;
using std::unique_ptr;


//===========================================
// loadStateId
//===========================================
int loadStateId(const AppConfig& conf) {
  int id = 0;
  ifstream fin(conf.userDataDir + sep + "procalc.dat", ifstream::binary);

  if (fin.good()) {
    fin.read(reinterpret_cast<char*>(&id), sizeof(id));
  }

  return id;
}

//===========================================
// persistStateId
//===========================================
void persistStateId(const AppConfig& conf, int id) {
  ofstream fout(conf.userDataDir + sep + "procalc.dat", ofstream::binary | ofstream::trunc);
  fout.write(reinterpret_cast<const char*>(&id), sizeof(id));
}

//===========================================
// main
//===========================================
int main(int argc, char** argv) {

#ifdef __APPLE__
  changeWorkingDir();
#endif

  try {
    AppConfig appConfig;

    int stateId = 0;

    if (argc > 1) {
      stateId = std::stoi(argv[1]);
    }
    else {
      stateId = loadStateId(appConfig);
    }

    DBG_PRINT("Loading app state " << stateId << "\n");

    EventSystem eventSystem;
    unique_ptr<RootSpec> rootSpec(makeRootSpec(stateId));

    Application app(argc, argv);

    MainFragment mainFragment(eventSystem);
    mainFragment.rebuild(rootSpec->mainFragmentSpec);
    mainFragment.show();

    eventSystem.listen("RequestStateChangeEvent", [&](const Event& e) {
      stateId = dynamic_cast<const RequestStateChangeEvent&>(e).stateId;

      rootSpec.reset(makeRootSpec(stateId));
      mainFragment.rebuild(rootSpec->mainFragmentSpec);
    });

    int code = app.exec();
    persistStateId(appConfig, stateId);

    return code;
  }
  catch (Exception& e) {
    cerr << "Encountered fatal error; " << e.what() << "\n";
    return EXIT_FAILURE;
  }
  catch (...) {
    cerr << "Encountered fatal error; Cause unknown\n";
    return EXIT_FAILURE;
  }
}
