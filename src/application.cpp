#include <stdexcept>
#include <iostream>
#include "application.hpp"


using namespace std;


//===========================================
// Application::Application
//===========================================
bool Application::notify(QObject* receiver, QEvent* event) {
  try {
    return QApplication::notify(receiver, event);
  }
  catch (exception& e) {
    cerr << "Encountered fatal error: " << e.what() << "\n";
    exit(EXIT_FAILURE);
  }
  catch (...) {
    cerr << "Encountered fatal error: Cause unknown\n";
    exit(EXIT_FAILURE);
  }

  return false;
}
