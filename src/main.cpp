#include <iostream>
#include "application.hpp"
#include "main_window.hpp"
#include "exception.hpp"
#include "app_config.hpp"
#include "app_state.hpp"
#include "event_system.hpp"
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


using namespace std;


//===========================================
// main
//===========================================
int main(int argc, char** argv) {

#ifdef __APPLE__
  changeWorkingDir();
#endif

  try {
    AppConfig appConfig;

    EventSystem eventSystem;

    AppState appState;
    appState.load(appConfig);

    Application app(argc, argv);
    MainWindow window(appConfig, appState, eventSystem);
    window.show();

    eventSystem.fire(Event("appStateUpdated"));

    return app.exec();
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
