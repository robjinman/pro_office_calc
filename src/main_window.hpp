#ifndef __PROCALC_MAIN_WINDOW_HPP__
#define __PROCALC_MAIN_WINDOW_HPP__


#include <QMainWindow>


class QAction;
class QWidget;
class AppConfig;
class AppState;

class MainWindow : public QMainWindow {
  Q_OBJECT

  public:
    MainWindow(const AppConfig& appConfig, AppState& state);

    virtual ~MainWindow();

  protected:
    virtual void closeEvent(QCloseEvent*);

  private slots:
    void showAbout();

  private:
    const AppConfig& m_appConfig;
    AppState& m_appState;

    QMenu* m_mnuFile;
    QMenu* m_mnuHelp;
    QAction* m_actAbout;
    QAction* m_actQuit;
    QWidget* m_wgtCentral;
};


#endif
