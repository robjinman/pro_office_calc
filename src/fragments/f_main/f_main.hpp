#ifndef __PROCALC_FRAGMENTS_F_MAIN_HPP__
#define __PROCALC_FRAGMENTS_F_MAIN_HPP__


#include <memory>
#include <functional>
#include <QMainWindow>
#include <QMenu>
#include <QAction>
#include "update_loop.hpp"
#include "fragment.hpp"


class UpdateLoop;
class EventSystem;


struct FMainData : public FragmentData {
  FMainData(EventSystem& eventSystem, UpdateLoop& updateLoop)
    : FragmentData(),
      eventSystem(eventSystem),
      updateLoop(updateLoop) {}

  EventSystem& eventSystem;
  UpdateLoop& updateLoop;
  std::unique_ptr<QMenu> mnuFile;
  std::unique_ptr<QAction> actQuit;
  std::unique_ptr<QMenu> mnuHelp;
  std::unique_ptr<QAction> actAbout;
  std::function<void()> fnOnQuit = []() {};
};


class FMain : public QMainWindow, public Fragment {
  Q_OBJECT

  public:
    FMain(EventSystem& eventSystem, UpdateLoop& updateLoop);

    virtual void reload(const FragmentSpec& spec) override;
    virtual void cleanUp() override;

    virtual ~FMain();

  protected:
    virtual void closeEvent(QCloseEvent*);

  private slots:
    void showAbout();

  private:
    FMainData m_data;
    QString m_aboutDialogTitle;
    QString m_aboutDialogText;
};


#endif
