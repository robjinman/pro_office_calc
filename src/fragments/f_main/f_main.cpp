#include <QMenuBar>
#include <QApplication>
#include "fragments/f_main/f_main.hpp"
#include "fragments/f_main/f_main_spec.hpp"
#include "utils.hpp"
#include "exception.hpp"
#include "effects.hpp"


using std::string;
using std::unique_ptr;


//===========================================
// FMain::FMain
//===========================================
FMain::FMain(EventSystem& eventSystem)
  : QMainWindow(nullptr),
    Fragment("FMain", m_data),
    m_data(eventSystem) {

  setFixedSize(300, 260);

  setWindowTitle("Pro Office Calculator");

  m_data.actQuit.reset(new QAction("Quit", this));

  m_data.mnuFile.reset(menuBar()->addMenu("File"));
  m_data.mnuFile->addAction(m_data.actQuit.get());

  m_data.mnuHelp.reset(menuBar()->addMenu("Help"));

  connect(m_data.actQuit.get(), SIGNAL(triggered()), this, SLOT(close()));

  QTimer* timer = new QTimer(this);
  connect(timer, SIGNAL(timeout()), this, SLOT(tick()));

  m_data.updateLoop.reset(new UpdateLoop(unique_ptr<QTimer>(timer), 50));
}

//===========================================
// FMain::rebuild
//===========================================
void FMain::rebuild(const FragmentSpec& spec_) {
  Fragment::rebuild(spec_);
}

//===========================================
// FMain::cleanUp
//===========================================
void FMain::cleanUp() {

}

//===========================================
// FMain::tick
//===========================================
void FMain::tick() {
  m_data.updateLoop->update();
}

//===========================================
// FMain::closeEvent
//===========================================
void FMain::closeEvent(QCloseEvent*) {
  m_data.fnOnQuit();
  DBG_PRINT("Quitting\n");
}

//===========================================
// FMain::~FMain
//===========================================
FMain::~FMain() {
  DBG_PRINT("FMain::~FMain()\n");
}
