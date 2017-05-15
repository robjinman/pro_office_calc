#include <QAction>
#include <QMenuBar>
#include <QMessageBox>
#include "event_system.hpp"
#include "request_state_change_event.hpp"
#include "exception.hpp"
#include "state_ids.hpp"
#include "fragments/normal_calculator_fragment.hpp"
#include "fragments/specs/normal_calculator_fragment_spec.hpp"


//===========================================
// NormalCalculatorFragment::NormalCalculatorFragment
//===========================================
NormalCalculatorFragment::NormalCalculatorFragment(QMenuBar& menuBar, QAction& actQuit,
  QAction& actAbout, EventSystem& eventSystem)
  : m_menuBar(menuBar),
    m_actQuit(actQuit),
    m_actAbout(actAbout),
    m_eventSystem(eventSystem) {}

//===========================================
// NormalCalculatorFragment::showAbout
//===========================================
void NormalCalculatorFragment::showAbout() {
  QMessageBox msgBox;
  msgBox.setTextFormat(Qt::RichText);
  msgBox.setWindowTitle("About");

  QString msg = "";
  msg = msg + "<p align='center'><big>Pro Office Calculator</big><br>Version 1.0.0</p>"
    "<p align='center'><a href='http://localhost'>Acme Inc</a></p>"
    "<p align='center'>Copyright (c) 2017 Acme Inc. All rights reserved.</p>"
    "<i>" + QString::number(10 - m_stateId) + "</i>";

  msgBox.setText(msg);
  msgBox.exec();
}

//===========================================
// NormalCalculatorFragment::quit
//===========================================
void NormalCalculatorFragment::quit() {
  m_eventSystem.fire(RequestStateChangeEvent(m_stateId + 1));
}

//===========================================
// NormalCalculatorFragment::rebuild
//===========================================
void NormalCalculatorFragment::rebuild(const NormalCalculatorFragmentSpec& spec) {
  m_stateId = spec.stateId;
  connect(&m_actAbout, SIGNAL(triggered()), this, SLOT(showAbout()));
  connect(&m_actQuit, SIGNAL(triggered()), this, SLOT(quit()));
}
