#ifndef __PROCALC_FRAGMENTS_NORMAL_CALCULATOR_FRAGMENT_HPP__
#define __PROCALC_FRAGMENTS_NORMAL_CALCULATOR_FRAGMENT_HPP__


#include <QObject>


class QMenuBar;
class QAction;
class EventSystem;
class NormalCalculatorFragmentSpec;

class NormalCalculatorFragment : public QObject {
  Q_OBJECT

  public:
    NormalCalculatorFragment(QMenuBar& menuBar, QAction& actQuit, QAction& actAbout,
      EventSystem& eventSystem);

    void rebuild(const NormalCalculatorFragmentSpec& spec);

  private slots:
    void showAbout();
    void quit();

  private:
    QMenuBar& m_menuBar;
    QAction& m_actAbout;
    EventSystem& m_eventSystem;

    int m_count;
};


#endif
