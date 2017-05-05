#ifndef __PROCALC_MAIN_WINDOW_HPP__
#define __PROCALC_MAIN_WINDOW_HPP__


#include <QMainWindow>

class QAction;
class QWidget;

class MainWindow : public QMainWindow {
  Q_OBJECT

  public:
    MainWindow(QWidget* parent = 0);

    virtual ~MainWindow();

  private slots:
    void showAbout();

  private:
    QMenu* m_mnuFile;
    QMenu* m_mnuHelp;
    QAction* m_actAbout;
    QAction* m_actQuit;
    QWidget* m_wgtCentral;
};


#endif
