#include <sstream>
#include <iomanip>
#include <math.h>
#include <QMenuBar>
#include <QApplication>
#include <QVBoxLayout>
#include <QMessageBox>
#include <QLineEdit>
#include <QTimer>
#include "main_window.hpp"
#include "button_grid.hpp"
#include "utils.hpp"
#include "types.hpp"
#include "app_config.hpp"
#include "main_state.hpp"
#include "calculator.hpp"
#include "exception.hpp"
#include "update_loop.hpp"


using std::string;
using std::stringstream;
using std::unique_ptr;


//===========================================
// idToOp
//===========================================
static operator_t idToOp(int id) {
  switch (id) {
    case BTN_PLUS: return OP_PLUS;
    case BTN_MINUS: return OP_MINUS;
    case BTN_TIMES: return OP_TIMES;
    case BTN_DIVIDE: return OP_DIVIDE;
    default: EXCEPTION("No operator corresponding to button id " << id);
  }
}

//===========================================
// formatNumber
//===========================================
static string formatNumber(double num) {
  stringstream ss;
  ss << std::fixed << std::setprecision(8) << num;
  string str = ss.str();

  std::size_t i = str.find('.');
  if (i != string::npos) {
    while (str.back() == '0') {
      str.pop_back();
    }

    if (str.back() == '.') {
      str.pop_back();
    }
  }

  return str;
}

//===========================================
// MainWindow::MainWindow
//===========================================
MainWindow::MainWindow(const AppConfig& appConfig, MainState& appState, EventSystem& eventSystem)
  : QMainWindow(nullptr),
    m_appConfig(appConfig),
    m_appState(appState),
    m_eventSystem(eventSystem) {

  setFixedSize(300, 260);

  setWindowTitle("Pro Office Calculator");

  QTimer* timer = new QTimer(this);
  connect(timer, SIGNAL(timeout()), this, SLOT(tick()));

  m_updateLoop.reset(new UpdateLoop(unique_ptr<QTimer>(timer), 100));

  m_actQuit.reset(new QAction("Quit", this));

  m_mnuFile.reset(menuBar()->addMenu("File"));
  m_mnuFile->addAction(m_actQuit.get());

  m_actAbout.reset(new QAction("About", this));
  m_mnuHelp.reset(menuBar()->addMenu("Help"));
  m_mnuHelp->addAction(m_actAbout.get());

  m_eventSystem.listen("appStateUpdated", [this](const Event& e) {
    this->onUpdateAppState(e);
  });

  m_wgtCentral.reset(new QWidget(this));
  setCentralWidget(m_wgtCentral.get());

  m_wgtDigitDisplay.reset(new QLineEdit(m_wgtCentral.get()));
  m_wgtDigitDisplay->setMaximumHeight(40);
  m_wgtDigitDisplay->setAlignment(Qt::AlignRight);
  m_wgtDigitDisplay->setReadOnly(true);

  m_wgtButtonGrid.reset(new ButtonGrid(m_appState, m_eventSystem,
    *m_updateLoop, m_wgtCentral.get()));

  QVBoxLayout* vbox = new QVBoxLayout;
  vbox->addWidget(m_wgtDigitDisplay.get());
  vbox->addWidget(m_wgtButtonGrid.get());
  m_wgtCentral->setLayout(vbox);

  connect(m_wgtButtonGrid.get(), SIGNAL(buttonClicked(int)), this, SLOT(buttonClicked(int)));
  connect(m_actQuit.get(), SIGNAL(triggered()), this, SLOT(close()));
  connect(m_actAbout.get(), SIGNAL(triggered()), this, SLOT(showAbout()));

  if (m_appState.level == MainState::LVL_NORMAL_CALCULATOR) {
    auto& subState = castSubstate<MainSub0State>();

    if (subState.openCount > 1) {
      subState.openCount--;
    }
    else {
      m_appState.subState.reset(new MainSub1State);
      m_appState.level = MainState::LVL_DANGER_INFINITY;
    }
  }
}

//===========================================
// MainWindow::onUpdateAppState
//===========================================
void MainWindow::onUpdateAppState(const Event& e) {
  if (m_appState.level > MainState::LVL_DANGER_INFINITY) {
    setWindowTitle("P̴r̵o̸ ̷O̸f̶f̸i̸c̷e̷ ̵C̶a̶l̶c̷u̷l̸a̸t̶o̷r̶");

    m_actQuit->setText("Q̶u̸i̶t̷");
    m_mnuFile->setTitle("F̷i̸l̴e̷");
    m_actAbout->setText("A̵b̵o̸u̸t̴");
    m_mnuHelp->setTitle("H̸e̵l̴p̷");
  }
}

//===========================================
// MainWindow::tick
//===========================================
void MainWindow::tick() {
  m_updateLoop->update();
}

//===========================================
// MainWindow::buttonClicked
//===========================================
void MainWindow::buttonClicked(int id) {
  static Calculator calculator;
  static bool reset = true;

  QString text = m_wgtDigitDisplay->text();

  if (id <= 9) {
    if (reset || calculator.op() != OP_NONE) {
      text = "";
      reset = false;
    }

    text.append(std::to_string(id).c_str());

    double value = std::strtod(text.toStdString().c_str(), nullptr);
    calculator.putValue(value);
  }
  else {
    switch (id) {
      case BTN_PLUS:
      case BTN_MINUS:
      case BTN_TIMES:
      case BTN_DIVIDE:
        calculator.putOperator(idToOp(id));
        break;
      case BTN_POINT:
        if (!text.contains(".")) {
          text.append(".");
        }
        break;
      case BTN_EQUALS: {
        double result = calculator.evaluate();
        text = formatNumber(result).c_str();
        reset = true;

        if (m_appState.level == MainState::LVL_DANGER_INFINITY && std::isinf(result)) {
          m_appState.subState.reset(new MainSub2State);
          m_appState.level++;
          m_eventSystem.fire(Event("appStateUpdated"));
        }

        break;
      }
      case BTN_CLEAR:
        calculator.clear();
        text = "";
        break;
    }
  }

  m_wgtDigitDisplay->setText(text);
}

//===========================================
// MainWindow::closeEvent
//===========================================
void MainWindow::closeEvent(QCloseEvent*) {
  DBG_PRINT("Quitting\n");
}

//===========================================
// MainWindow::showAbout
//===========================================
void MainWindow::showAbout() {
  QMessageBox msgBox(this);
  msgBox.setTextFormat(Qt::RichText);

  QString msg;
  switch (m_appState.level) {
    case MainState::LVL_NORMAL_CALCULATOR:
      msg = msg + "<p align='center'><big>Pro Office Calculator</big><br>Version 1.0.0</p>"
        "<p align='center'><a href='http://localhost'>Acme Inc</a></p>"
        "<p align='center'>Copyright (c) 2017 Acme Inc. All rights reserved.</p>"
        "<i>" + QString::number(castSubstate<MainSub0State>().openCount) + "</i>";
      msgBox.setWindowTitle("About");
      break;
    case MainState::LVL_DANGER_INFINITY:
      msg = msg + "<p align='center'><big>Pro Office Calculator</big><br>Version 1.0.0</p>"
        "<p align='center'><a href='http://localhost'>Acme Inc</a></p>"
        "<p align='center'>Copyright (c) 2017 Acme Inc. All rights reserved.</p>"
        "<font size=6>⚠∞</font>";
      msgBox.setWindowTitle("About");
      break;
    case MainState::LVL_WEIRD:
      msg = msg + "<p align='center'><big>P̴r̵o̸ ̷O̸f̶f̸i̸c̷e̷ ̵C̶a̶l̶c̷u̷l̸a̸t̶o̷r̶</big><br>V̶e̸r̵s̴i̵o̸n̵ ̶1̸.̵0̵.̶0̵</p>"
        "<p><br><br></p>"
        "<p align='center'>C̴̠̓́o̶̥̟͘p̸̝͌̿y̷͈͈͝r̸̰͒ĭ̵͖͒g̸̭̭̅ḧ̸̟̖́͝ṯ̴̓ ̷̦̳̇(̷̯̩͆̈c̵͉̔)̵̛͕̕ ̴͔̇2̷̢̑͝0̵̖̀̎1̴̤̘͛7̷̻̳͛͌ ̷̙̙̈́̂Ạ̶̛̔c̸̢̿m̴̬̯̆͌ě̸̪͓͛ ̸̝͇̓͛I̸̺̜͊̀n̶̺̗̓̑c̸̳͖̆͋.̴͇͇̔̀ ̴̠́A̸̤̾̀l̸̛̜̖ḽ̴̈̾ ̴̹͌r̵̬̭̔͒i̴̭̣͛̔ǵ̵̩h̵̗̜͋̈́t̴̢̃s̶͇͓̆̅ ̷̗̀̚r̷̡͖͌͐ě̶̦̾ś̷͇̠e̴̫̺͒r̵̰͆v̵̧͝e̶̟͑ḓ̸̉.̵̨͉͌</p>";
        msgBox.setWindowTitle("A̶b̵o̷u̴t̶");
      break;
  }

  msgBox.setText(msg);
  msgBox.exec();
}

//===========================================
// MainWindow::~MainWindow
//===========================================
MainWindow::~MainWindow() {
  DBG_PRINT("MainWindow::~MainWindow()\n");
}
