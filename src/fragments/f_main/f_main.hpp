#ifndef __PROCALC_FRAGMENTS_F_MAIN_HPP__
#define __PROCALC_FRAGMENTS_F_MAIN_HPP__


#include <memory>
#include <QMainWindow>
#include <functional>
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
  std::function<void()> fnOnQuit = []() {};
};


class FMain : public QMainWindow, public Fragment {
  Q_OBJECT

  public:
    FMain(EventSystem& eventSystem, UpdateLoop& updateLoop);

    virtual void rebuild(const FragmentSpec& spec) override;
    virtual void cleanUp() override;

    virtual ~FMain();

  protected:
    virtual void closeEvent(QCloseEvent*);

  private:
    FMainData m_data;
};


#endif
