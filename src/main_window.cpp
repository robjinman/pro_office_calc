#include <QMenuBar>
#include <QApplication>
#include <QVBoxLayout>
#include <QMessageBox>
#include <QTextEdit>
#include "main_window.hpp"
#include "button_grid.hpp"
#include "utils.hpp"
#include "types.hpp"
#include "app_config.hpp"
#include "app_state.hpp"


using std::string;
using std::unique_ptr;
using std::to_string;


//===========================================
// numberToWord
//===========================================
static QString numberToWord(int n) {
  switch (n) {
    case 1: return "one";
    case 2: return "two";
    case 3: return "three";
    case 4: return "four";
    case 5: return "five";
    case 6: return "six";
    case 7: return "seven";
    case 8: return "eight";
    case 9: return "nine";
    case 10: return "ten";
    default: return "?";
  }
}

//===========================================
// MainWindow::MainWindow
//===========================================
MainWindow::MainWindow(const AppConfig& appConfig, AppState& appState)
  : QMainWindow(nullptr),
    m_appConfig(appConfig),
    m_appState(appState) {

  setFixedSize(300, 260);

  setWindowTitle("Pro Office Calculator");

  m_actQuit.reset(new QAction("Quit", this));

  m_mnuFile.reset(menuBar()->addMenu("File"));
  m_mnuFile->addAction(m_actQuit.get());

  m_actAbout.reset(new QAction("About", this));
  m_mnuHelp.reset(menuBar()->addMenu("Help"));
  m_mnuHelp->addAction(m_actAbout.get());

  m_wgtCentral.reset(new QWidget(this));
  setCentralWidget(m_wgtCentral.get());

  m_wgtDigitDisplay.reset(new QTextEdit(m_wgtCentral.get()));
  m_wgtDigitDisplay->setMaximumHeight(40);

  m_wgtButtonGrid.reset(new ButtonGrid(m_wgtCentral.get()));

  QVBoxLayout* vbox = new QVBoxLayout;
  vbox->addWidget(m_wgtDigitDisplay.get());
  vbox->addWidget(m_wgtButtonGrid.get());
  m_wgtCentral->setLayout(vbox);

  connect(m_wgtButtonGrid.get(), SIGNAL(buttonClicked(int)), this, SLOT(buttonClicked(int)));
  connect(m_actQuit.get(), SIGNAL(triggered()), this, SLOT(close()));
  connect(m_actAbout.get(), SIGNAL(triggered()), this, SLOT(showAbout()));
}

//===========================================
// MainWindow::buttonClicked
//===========================================
void MainWindow::buttonClicked(int id) {
  DBG_PRINT("Button " << id << " clicked\n");
}

//===========================================
// MainWindow::closeEvent
//===========================================
void MainWindow::closeEvent(QCloseEvent*) {
  DBG_PRINT("Quitting\n");

  m_appState.count--;
  m_appState.persist(m_appConfig);
}

//===========================================
// MainWindow::showAbout
//===========================================
void MainWindow::showAbout() {
   QString msg;
   msg = msg + "<p align='center'><big>Pro Office Calculator</big><br>Version 1.0.0</p>"
     "<p align='center'><a href='http://localhost'>Acme Inc</a></p>"
     "<p align='center'>Copyright (c) 2017 Acme Inc. All rights reserved.</p>"
     "<i>" + numberToWord(m_appState.count) + "</i>";

  QMessageBox msgBox(this);
  msgBox.setWindowTitle("About");
  msgBox.setTextFormat(Qt::RichText);
  msgBox.setText(msg);
  msgBox.exec();
}

//===========================================
// MainWindow::~MainWindow
//===========================================
MainWindow::~MainWindow() {
  DBG_PRINT("MainWindow::~MainWindow()\n");
}
