#ifndef __PROCALC_MAIN_WINDOW_HPP__
#define __PROCALC_MAIN_WINDOW_HPP__


#include <memory>
#include <vector>
#include <QMainWindow>
#include "event_system.hpp"
#include "main_state.hpp"


class QAction;
class QWidget;
class QLineEdit;
class UpdateLoop;
class ButtonGrid;
class AppConfig;

class MainWindow : public QMainWindow {
  Q_OBJECT

  public:
    MainWindow(const AppConfig& appConfig, MainState& state, EventSystem& eventSystem);

    virtual ~MainWindow();

  protected:
    virtual void closeEvent(QCloseEvent*);

  private slots:
    void showAbout();
    void buttonClicked(int id);
    void tick();

  private:
    void onUpdateAppState(const Event& e);

    template<class T>
    T& castSubstate() {
      return dynamic_cast<T&>(*m_appState.subState);
    }

    const AppConfig& m_appConfig;
    MainState& m_appState;
    EventSystem& m_eventSystem;
    std::unique_ptr<UpdateLoop> m_updateLoop;

    std::unique_ptr<QMenu> m_mnuFile;
    std::unique_ptr<QMenu> m_mnuHelp;
    std::unique_ptr<QAction> m_actAbout;
    std::unique_ptr<QAction> m_actQuit;
    std::unique_ptr<QWidget> m_wgtCentral;
    std::unique_ptr<QLineEdit> m_wgtDigitDisplay;
    std::unique_ptr<ButtonGrid> m_wgtButtonGrid;
};


#endif
