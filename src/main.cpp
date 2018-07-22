#include <iostream>
#include <memory>
#include <thread>
#include <QDir>
#include "application.hpp"
#include "exception.hpp"
#include "app_config.hpp"
#include "f_main_spec_factory.hpp"
#include "event_system.hpp"
#include "update_loop.hpp"
#include "platform.hpp"
#include "utils.hpp"
#include "fragments/f_main/f_main.hpp"
#include "fragments/f_main/f_main_spec.hpp"


using std::cerr;
using std::unique_ptr;


//===========================================
// main
//===========================================
int main(int argc, char** argv) {
  try {
    Application app(argc, argv);

#ifdef __APPLE__
    QDir dir(QCoreApplication::applicationDirPath());
    dir.cdUp();
    dir.cd("Plugins");
    QCoreApplication::setLibraryPaths(QStringList(dir.absolutePath()));
#endif

    AppConfig appConfig{argc, argv};

    DBG_PRINT("App version: " << appConfig.version << "\n");
    DBG_PRINT("App state: " << appConfig.stateId << "\n");
    DBG_PRINT("Hardware concurrency: " << std::thread::hardware_concurrency() << "\n");

    std::shared_ptr<EventSystem> eventSystem{new EventSystem};
    UpdateLoop updateLoop(50);

    unique_ptr<FMainSpec> mainSpec(makeFMainSpec(appConfig));

    FMain mainFragment({appConfig, *eventSystem, updateLoop});
    mainFragment.rebuild(*mainSpec, false);
    mainFragment.show();

    EventHandle hQuit = eventSystem->listen("quit", [](const Event&) {
      QApplication::exit(0);
    });

    EventHandle hSetConfigParam = eventSystem->listen("setConfigParam", [&](const Event& e_) {
      auto& e = dynamic_cast<const SetConfigParamEvent&>(e_);
      appConfig.setParam(e.name, e.value);
    });

    EventHandle hStateChange = eventSystem->listen("requestStateChange", [&](const Event& e_) {
      const RequestStateChangeEvent& e = dynamic_cast<const RequestStateChangeEvent&>(e_);
      appConfig.stateId = e.stateId;

      updateLoop.finishAll();
      app.processEvents();

      mainSpec.reset(makeFMainSpec(appConfig));
      mainFragment.rebuild(*mainSpec, e.hardReset);
    });

    int code = app.exec();
    appConfig.persistState();

    return code;
  }
  catch (std::exception& e) {
    cerr << "Encountered fatal error; " << e.what() << "\n";
    return EXIT_FAILURE;
  }
  catch (...) {
    cerr << "Encountered fatal error; Cause unknown\n";
    return EXIT_FAILURE;
  }
}
