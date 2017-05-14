#ifndef __PROCALC_MAIN_WINDOW_HPP__
#define __PROCALC_MAIN_WINDOW_HPP__


#include <memory>
#include <vector>
#include <QMainWindow>
#include "event_system.hpp"
#include "view.hpp"


class QAction;
class QWidget;
class QLineEdit;
class UpdateLoop;
class ButtonGrid;
class AppConfig;
class MainState;

class MainView : public QMainWindow, public View {
  Q_OBJECT

  public:
    MainView(MainState& appState, const AppConfig& appConfig, EventSystem& eventSystem);

    virtual void setup(int rootState) override;

    virtual ~MainView();

  protected:
    virtual void closeEvent(QCloseEvent*);

  private slots:
    void showAbout();
    void buttonClicked(int id);
    void tick();

  private:
    void onUpdateAppState(const Event& e);

    MainState& appstate;
    const AppConfig& m_appConfig;
    EventSystem& m_eventSystem;
    std::unique_ptr<UpdateLoop> m_updateLoop;

    std::unique_ptr<QMenu> m_mnuFile;
    std::unique_ptr<QMenu> m_mnuHelp;
    std::unique_ptr<QAction> m_actAbout;
    std::unique_ptr<QAction> m_actQuit;
    std::unique_ptr<QWidget> m_wgtCentral;

    std::unique_ptr<View> m_subview;
};


#endif
