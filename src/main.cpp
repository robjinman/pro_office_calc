#include <iostream>
#include <fstream>
#include <memory>
#include "application.hpp"
#include "main_window.hpp"
#include "exception.hpp"
#include "app_config.hpp"
#include "app_state.hpp"
#include "event_system.hpp"
#include "platform.hpp"
#include "utils.hpp"
#ifdef __APPLE__
#include "CoreFoundation/CoreFoundation.h"
#endif


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
// loadAppState
//===========================================
void loadAppState(const AppConfig& conf, AppState& appState) {
  ifstream fin(conf.userDataDir + sep + "procalc.dat", ifstream::binary);

  if (fin.good()) {
    appState.deserialize(fin);
  }
}

//===========================================
// persistAppState
//===========================================
void persistAppState(const AppConfig& conf, AppState& appState) {
  ofstream fout(conf.userDataDir + sep + "procalc.dat", ofstream::binary | ofstream::trunc);
  appState.serialize(fout);
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

    MainState appState;
    loadAppState(appConfig, appState);

    EventSystem eventSystem;

    Application app(argc, argv);
    MainWindow window(appConfig, appState, eventSystem);
    window.show();

    eventSystem.fire(Event("appStateUpdated"));

    int code = app.exec();
    persistAppState(appConfig, appState);

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
