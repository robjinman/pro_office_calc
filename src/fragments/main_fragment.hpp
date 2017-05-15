#ifndef __PROCALC_FRAGMENTS_MAIN_FRAGMENT_HPP__
#define __PROCALC_FRAGMENTS_MAIN_FRAGMENT_HPP__


#include <memory>
#include <vector>
#include <QMainWindow>


class QAction;
class UpdateLoop;
class AppConfig;
class EventSystem;
class CalculatorFragment;
class MainFragmentSpec;

class MainFragment : public QMainWindow {
  Q_OBJECT

  public:
    MainFragment(EventSystem& eventSystem);

    void rebuild(const MainFragmentSpec& spec);

    virtual ~MainFragment();

  protected:
    virtual void closeEvent(QCloseEvent*);

  private slots:
    void tick();

  private:
    EventSystem& m_eventSystem;
    std::unique_ptr<UpdateLoop> m_updateLoop;

    std::unique_ptr<QMenu> m_mnuFile;
    std::unique_ptr<QMenu> m_mnuHelp;
    std::unique_ptr<QAction> m_actAbout;
    std::unique_ptr<QAction> m_actQuit;

    std::unique_ptr<CalculatorFragment> m_calculatorFragment;
};


#endif
