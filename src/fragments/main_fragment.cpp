#include <math.h>
#include <QMenuBar>
#include <QApplication>
#include <QMessageBox>
#include <QTimer>
#include "utils.hpp"
#include "types.hpp"
#include "exception.hpp"
#include "update_loop.hpp"
#include "fragments/calculator_fragment.hpp"
#include "fragments/main_fragment.hpp"
#include "fragments/specs/main_fragment_spec.hpp"


using std::string;
using std::unique_ptr;


//===========================================
// MainFragment::MainFragment
//===========================================
MainFragment::MainFragment(EventSystem& eventSystem)
  : QMainWindow(nullptr),
    m_eventSystem(eventSystem) {

  setFixedSize(300, 260);

  setWindowTitle("Pro Office Calculator");

  QTimer* timer = new QTimer(this);
  connect(timer, SIGNAL(timeout()), this, SLOT(tick()));

  m_updateLoop.reset(new UpdateLoop(unique_ptr<QTimer>(timer), 50));
}

//===========================================
// MainFragment::rebuild
//===========================================
void MainFragment::rebuild(const MainFragmentSpec& spec) {
  m_actQuit.reset(new QAction("Quit", this));

  m_mnuFile.reset(menuBar()->addMenu("File"));
  m_mnuFile->addAction(m_actQuit.get());

  m_actAbout.reset(new QAction("About", this));
  m_mnuHelp.reset(menuBar()->addMenu("Help"));
  m_mnuHelp->addAction(m_actAbout.get());

  connect(m_actQuit.get(), SIGNAL(triggered()), this, SLOT(close()));
  //connect(m_actAbout.get(), SIGNAL(triggered()), this, SLOT(showAbout()));

  if (spec.showCalculatorFragment) {
    if (!m_calculatorFragment) {
      m_calculatorFragment.reset(new CalculatorFragment(this, *menuBar(), *m_actAbout, m_eventSystem,
        *m_updateLoop));
    }

    m_calculatorFragment->rebuild(spec.calculatorFragmentSpec);
    setCentralWidget(m_calculatorFragment.get());
  }
  else {
    m_calculatorFragment.reset();
  }
/*
  if (m_appState.rootState > ST_DANGER_INFINITY) {
    setWindowTitle("P̴r̵o̸ ̷O̸f̶f̸i̸c̷e̷ ̵C̶a̶l̶c̷u̷l̸a̸t̶o̷r̶");

    m_actQuit->setText("Q̶u̸i̶t̷");
    m_mnuFile->setTitle("F̷i̸l̴e̷");
    m_actAbout->setText("A̵b̵o̸u̸t̴");
    m_mnuHelp->setTitle("H̸e̵l̴p̷");
  }*/
}

//===========================================
// MainFragment::tick
//===========================================
void MainFragment::tick() {
  m_updateLoop->update();
}

//===========================================
// MainFragment::closeEvent
//===========================================
void MainFragment::closeEvent(QCloseEvent*) {
  DBG_PRINT("Quitting\n");
}

//===========================================
// MainFragment::showAbout
//===========================================
/*
void MainFragment::showAbout() {
  QMessageBox msgBox(this);
  msgBox.setTextFormat(Qt::RichText);

  QString msg;
  if (ltelte<int>(ST_NORMAL_CALCULATOR_0, m_appState.rootState, ST_NORMAL_CALCULATOR_10)) {
    msg = msg + "<p align='center'><big>Pro Office Calculator</big><br>Version 1.0.0</p>"
      "<p align='center'><a href='http://localhost'>Acme Inc</a></p>"
      "<p align='center'>Copyright (c) 2017 Acme Inc. All rights reserved.</p>"
      "<i>" + QString::number(castSubstate<MainSub0State>().openCount) + "</i>";
    msgBox.setWindowTitle("About");
  }
  else {
    switch (m_appState.rootState) {
      case ST_DANGER_INFINITY:
        msg = msg + "<p align='center'><big>Pro Office Calculator</big><br>Version 1.0.0</p>"
          "<p align='center'><a href='http://localhost'>Acme Inc</a></p>"
          "<p align='center'>Copyright (c) 2017 Acme Inc. All rights reserved.</p>"
          "<font size=6>⚠∞</font>";
        msgBox.setWindowTitle("About");
        break;
      case ST_SHUFFLED_KEYS:
        msg = msg + "<p align='center'><big>P̴r̵o̸ ̷O̸f̶f̸i̸c̷e̷ ̵C̶a̶l̶c̷u̷l̸a̸t̶o̷r̶</big><br>V̶e̸r̵s̴i̵o̸n̵ ̶1̸.̵0̵.̶0̵</p>"
          "<h2 align='center'>4071</h2>"
          "<p align='center'>C̵o̶p̷y̴r̴i̵g̷h̷t̸ ̶(̶c̵)̷ ̶1̵9̵9̸3̸ ̷A̵c̸m̸e̶ ̷I̶n̵c̷.̷ ̸A̵l̴l̷ ̶r̸i̶g̸h̷t̴s̶ ̴r̸e̶s̴e̶r̸v̵e̶d̶.̶</p>";
          msgBox.setWindowTitle("A̶b̵o̷u̴t̶");
        break;
    }
  }

  msgBox.setText(msg);
  msgBox.exec();
}*/

//===========================================
// MainFragment::~MainFragment
//===========================================
MainFragment::~MainFragment() {
  DBG_PRINT("MainFragment::~MainFragment()\n");
}
