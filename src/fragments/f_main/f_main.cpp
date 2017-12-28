#include <QMenuBar>
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
FMain::FMain(EventSystem& eventSystem, UpdateLoop& updateLoop)
  : QMainWindow(nullptr),
    Fragment("FMain", m_data),
    m_data(eventSystem, updateLoop) {

  DBG_PRINT("FMain::FMain\n");
}

//===========================================
// FMain::initialise
//===========================================
void FMain::initialise(const FragmentSpec& spec_) {
  DBG_PRINT("FMain::initialise\n");

  setWindowTitle("Pro Office Calculator");

  m_data.mnuFile.reset(menuBar()->addMenu("File"));
  m_data.actQuit.reset(new QAction("Quit", this));
  m_data.mnuFile->addAction(m_data.actQuit.get());

  connect(m_data.actQuit.get(), SIGNAL(triggered()), this, SLOT(close()));
}

//===========================================
// FMain::reload
//===========================================
void FMain::reload(const FragmentSpec& spec_) {
  DBG_PRINT("FMain::reload\n");

  auto& spec = dynamic_cast<const FMainSpec&>(spec_);

  setFixedSize(spec.width, spec.height);

  m_data.mnuFile->setTitle(spec.fileLabel);
  m_data.actQuit->setText(spec.quitLabel);

  setColour(*this, spec.bgColour, QPalette::Window);
  setWindowTitle(spec.windowTitle);
}

//===========================================
// FMain::closeEvent
//===========================================
void FMain::closeEvent(QCloseEvent*) {
  m_data.fnOnQuit();
  DBG_PRINT("Quitting\n");
}

//===========================================
// FMain::cleanUp
//===========================================
void FMain::cleanUp() {
  DBG_PRINT("FMain::cleanUp\n");
}

//===========================================
// FMain::~FMain
//===========================================
FMain::~FMain() {
  DBG_PRINT("FMain::~FMain()\n");
}
