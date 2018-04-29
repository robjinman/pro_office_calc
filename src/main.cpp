#include <iostream>
#include <fstream>
#include <memory>
#include "application.hpp"
#include "exception.hpp"
#include "app_config.hpp"
#include "f_main_spec_factory.hpp"
#include "event_system.hpp"
#include "update_loop.hpp"
#include "platform.hpp"
#include "utils.hpp"
#include "request_state_change_event.hpp"
#include "fragments/f_main/f_main.hpp"
#include "fragments/f_main/f_main_spec.hpp"


using std::ifstream;
using std::ofstream;
using std::cout;
using std::cerr;
using std::unique_ptr;


//===========================================
// loadStateId
//===========================================
int loadStateId() {
  int id = 0;
  ifstream fin(config::saveDataPath("procalc.dat"), ifstream::binary);

  if (fin.good()) {
    fin.read(reinterpret_cast<char*>(&id), sizeof(id));
  }

  return id;
}

//===========================================
// persistStateId
//===========================================
void persistStateId(int id) {
  DBG_PRINT("Persisting state id " << id << "\n");

  ofstream fout(config::saveDataPath("procalc.dat"), ofstream::binary | ofstream::trunc);
  fout.write(reinterpret_cast<const char*>(&id), sizeof(id));
}

//===========================================
// main
//===========================================
int main(int argc, char** argv) {
  try {
    int stateId = 0;

    if (argc > 1) {
      stateId = std::stoi(argv[1]);
    }
    else {
      stateId = loadStateId();
    }

    DBG_PRINT("Loading app state " << stateId << "\n");

    EventSystem eventSystem;
    UpdateLoop updateLoop(50);

    unique_ptr<FMainSpec> mainSpec(makeFMainSpec(stateId));

    Application app(argc, argv);

    FMain mainFragment({eventSystem, updateLoop});
    mainFragment.rebuild(*mainSpec, false);
    mainFragment.show();

    int quitEventId = -1;
    eventSystem.listen("quit", [](const Event&) {
      QApplication::exit(0);
    }, quitEventId);

    int requestStateChangeEventId = -1;
    eventSystem.listen("requestStateChange", [&](const Event& e_) {
      const RequestStateChangeEvent& e = dynamic_cast<const RequestStateChangeEvent&>(e_);
      stateId = e.stateId;

      updateLoop.finishAll();
      app.processEvents();

      mainSpec.reset(makeFMainSpec(stateId));
      mainFragment.rebuild(*mainSpec, e.hardReset);
    }, requestStateChangeEventId);

    int code = app.exec();
    persistStateId(stateId);

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
