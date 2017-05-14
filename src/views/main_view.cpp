#include <math.h>
#include <QMenuBar>
#include <QApplication>
#include <QVBoxLayout>
#include <QMessageBox>
#include <QLineEdit>
#include <QTimer>
#include "button_grid.hpp"
#include "utils.hpp"
#include "types.hpp"
#include "app_config.hpp"
#include "calculator.hpp"
#include "exception.hpp"
#include "update_loop.hpp"
#include "effects.hpp"
#include "state_ids.hpp"
#include "states/main_state.hpp"
#include "views/calculator_view.hpp"
#include "views/main_view.hpp"


using std::string;
using std::unique_ptr;


//===========================================
// MainView::MainView
//===========================================
MainView::MainView(MainState& appState, const AppConfig& appConfig, EventSystem& eventSystem)
  : QMainWindow(nullptr),
    m_appState(appState),
    m_appConfig(appConfig),
    m_eventSystem(eventSystem) {

  setFixedSize(300, 260);

  setWindowTitle("Pro Office Calculator");

  QTimer* timer = new QTimer(this);
  connect(timer, SIGNAL(timeout()), this, SLOT(tick()));

  m_updateLoop.reset(new UpdateLoop(unique_ptr<QTimer>(timer), 50));
}

//===========================================
// MainView::setup
//===========================================
void MainView::setup(int rootState) {
  m_actQuit.reset(new QAction("Quit", this));

  m_mnuFile.reset(menuBar()->addMenu("File"));
  m_mnuFile->addAction(m_actQuit.get());

  m_actAbout.reset(new QAction("About", this));
  m_mnuHelp.reset(menuBar()->addMenu("Help"));
  m_mnuHelp->addAction(m_actAbout.get());

  m_wgtCentral.reset(new QWidget(this));
  setCentralWidget(m_wgtCentral.get());

  connect(m_wgtButtonGrid.get(), SIGNAL(buttonClicked(int)), this, SLOT(buttonClicked(int)));
  connect(m_actQuit.get(), SIGNAL(triggered()), this, SLOT(close()));
  connect(m_actAbout.get(), SIGNAL(triggered()), this, SLOT(showAbout()));

  if (ltelte<int>(ST_NORMAL_CALCULATOR_0, rootState, ST_NORMAL_CALCULATOR_10)) {
    m_subview.reset(new CalculatorView(*this));
  }
  else {
    switch (rootState) {
      case ST_DANGER_INFINITY:
      case ST_SHUFFLED_KEYS:
        if (dynamic_cast<CalculatorView*>(m_subview.get()) == nullptr) {
          m_subview.reset(new CalculatorView(*this, m_eventSystem, *m_updateLoop));
        }
        break;
      default:
        EXCEPTION("MainState cannot be initialised for rootState " << rootState);
        break;
    }
  }

  m_subview->setup(rootState);

  if (m_appState.rootState > ST_DANGER_INFINITY) {
    setWindowTitle("P̴r̵o̸ ̷O̸f̶f̸i̸c̷e̷ ̵C̶a̶l̶c̷u̷l̸a̸t̶o̷r̶");

    m_actQuit->setText("Q̶u̸i̶t̷");
    m_mnuFile->setTitle("F̷i̸l̴e̷");
    m_actAbout->setText("A̵b̵o̸u̸t̴");
    m_mnuHelp->setTitle("H̸e̵l̴p̷");
  }
}

//===========================================
// MainView::tick
//===========================================
void MainView::tick() {
  m_updateLoop->update();
}

//===========================================
// MainView::buttonClicked
//===========================================
void MainView::buttonClicked(int id) {
  static Calculator calculator;

  if (id <= 9) {
    calculator.number(id);
  }
  else {
    switch (id) {
      case BTN_PLUS:
        calculator.plus();
        break;
      case BTN_MINUS:
        calculator.minus();
        break;
      case BTN_TIMES:
        calculator.times();
        break;
      case BTN_DIVIDE:
        calculator.divide();
        break;
      case BTN_POINT:
        calculator.point();
        break;
      case BTN_EQUALS: {
        double result = calculator.equals();

        if (m_appState.rootState == ST_DANGER_INFINITY && std::isinf(result)) {
          QColor origCol = palette().color(QPalette::Window);

          int frames = m_updateLoop->fps() / 2;
          m_updateLoop->add([=]() {
            static int i = -1;
            ++i;
            if (i % 2) {
              setColour(*this, origCol, QPalette::Window);
            }
            else {
              setColour(*this, Qt::white, QPalette::Window);
            }

            return i < frames;
          }, [&]() {
            transitionColour(*m_updateLoop, *this, QColor(160, 160, 160), QPalette::Window, 0.5,
              [&]() {
                m_appState.rootState = ST_SHUFFLED_KEYS;
                m_eventSystem.fire(Event("appStateUpdated"));
              });

            transitionColour(*m_updateLoop, *m_wgtDigitDisplay, QColor(160, 120, 120),
              QPalette::Base, 0.5);
          });
        }

        break;
      }
      case BTN_CLEAR:
        calculator.clear();
        break;
    }
  }

  m_wgtDigitDisplay->setText(calculator.display().c_str());
}

//===========================================
// MainView::closeEvent
//===========================================
void MainView::closeEvent(QCloseEvent*) {
  DBG_PRINT("Quitting\n");
}

//===========================================
// MainView::showAbout
//===========================================
void MainView::showAbout() {
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
}

//===========================================
// MainView::~MainView
//===========================================
MainView::~MainView() {
  DBG_PRINT("MainView::~MainView()\n");
}
