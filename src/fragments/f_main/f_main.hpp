#ifndef __PROCALC_FRAGMENTS_F_MAIN_HPP__
#define __PROCALC_FRAGMENTS_F_MAIN_HPP__


#include <memory>
#include <functional>
#include <QMainWindow>
#include <QMenu>
#include <QTimer>
#include "update_loop.hpp"
#include "fragment.hpp"


class QAction;
class UpdateLoop;
class AppConfig;
class EventSystem;
class FMainSpec;


struct FMainData : public FragmentData {
  FMainData(EventSystem& eventSystem)
    : FragmentData(),
      eventSystem(eventSystem) {}

  EventSystem& eventSystem;
  std::unique_ptr<UpdateLoop> updateLoop;

  std::unique_ptr<QMenu> mnuFile;
  std::unique_ptr<QMenu> mnuHelp;
  std::unique_ptr<QAction> actQuit;

  std::function<void()> fnOnQuit = []() {};
};


class FMain : public QMainWindow, public Fragment {
  Q_OBJECT

  public:
    FMain(EventSystem& eventSystem);

    virtual void rebuild(const FragmentSpec& spec) override;
    virtual void cleanUp() override;

    virtual ~FMain();

  protected:
    virtual void closeEvent(QCloseEvent*);

  private slots:
    void tick();

  private:
    FMainData m_data;
};


#endif
