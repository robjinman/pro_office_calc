#include <QMenuBar>
#include <QApplication>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QMessageBox>
#include "strings.hpp"
#include "main_window.hpp"


using namespace std;


//===========================================
// MainWindow::MainWindow
//===========================================
MainWindow::MainWindow(QWidget* parent)
  : QMainWindow(parent),
    m_mnuFile(nullptr),
    m_mnuHelp(nullptr),
    m_actAbout(nullptr),
    m_actQuit(nullptr),
    m_wgtCentral(nullptr) {

  resize(600, 300);

  m_actQuit = new QAction("Quit", this);

  m_mnuFile = menuBar()->addMenu("File");
  m_mnuFile->addAction(m_actQuit);

  m_actAbout = new QAction("About", this);
  m_mnuHelp = menuBar()->addMenu("Help");
  m_mnuHelp->addAction(m_actAbout);

  m_wgtCentral = new QWidget(this);
  setCentralWidget(m_wgtCentral);

  QHBoxLayout* hbox = new QHBoxLayout;
  m_wgtCentral->setLayout(hbox);

  connect(m_actQuit, SIGNAL(triggered()), qApp, SLOT(quit()));
  connect(m_actAbout, SIGNAL(triggered()), this, SLOT(showAbout()));
}

//===========================================
// MainWindow::showAbout
//===========================================
void MainWindow::showAbout() {
  QMessageBox::about(this, "This is the title", "This is the message");
}

//===========================================
// MainWindow::~MainWindow
//===========================================
MainWindow::~MainWindow() {
  delete m_wgtCentral;

  delete m_mnuFile;
  delete m_mnuHelp;
  delete m_actQuit;
  delete m_actAbout;
}
