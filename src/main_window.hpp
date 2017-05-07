#ifndef __PROCALC_MAIN_WINDOW_HPP__
#define __PROCALC_MAIN_WINDOW_HPP__


#include <memory>
#include <vector>
#include <QMainWindow>


class QAction;
class QWidget;
class QLineEdit;
class UpdateLoop;
class ButtonGrid;
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
    void buttonClicked(int id);
    void tick();

  private:
    const AppConfig& m_appConfig;
    AppState& m_appState;

    std::unique_ptr<QMenu> m_mnuFile;
    std::unique_ptr<QMenu> m_mnuHelp;
    std::unique_ptr<QAction> m_actAbout;
    std::unique_ptr<QAction> m_actQuit;
    std::unique_ptr<QWidget> m_wgtCentral;
    std::unique_ptr<QLineEdit> m_wgtDigitDisplay;
    std::unique_ptr<ButtonGrid> m_wgtButtonGrid;
    std::unique_ptr<UpdateLoop> m_updateLoop;
};


#endif
