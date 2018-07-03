#ifndef __PROCALC_FRAGMENTS_F_CONFIG_MAZE_HPP__
#define __PROCALC_FRAGMENTS_F_CONFIG_MAZE_HPP__


#include <array>
#include <QGridLayout>
#include <QPushButton>
#include <QLabel>
#include <QStackedLayout>
#include "fragments/f_main/f_settings_dialog/f_config_maze/are_you_sure_widget.hpp"
#include "fragments/f_main/f_settings_dialog/f_config_maze/config_page.hpp"
#include "console_widget.hpp"
#include "fragment.hpp"
#include "qt_obj_ptr.hpp"


struct FConfigMazeData : public FragmentData {
  QtObjPtr<QStackedLayout> stackedLayout;

  struct {
    QtObjPtr<QWidget> widget;
    QtObjPtr<QVBoxLayout> vbox;
    QtObjPtr<ConsoleWidget> wgtConsole;
    QtObjPtr<QPushButton> wgtBack;
  } consolePage;

  struct {
    QtObjPtr<QWidget> widget;
    QtObjPtr<QVBoxLayout> vbox;
    QtObjPtr<QPushButton> wgtToConsole;
  } consoleLaunchPage;

  struct {
    QtObjPtr<QWidget> widget;
    QtObjPtr<QVBoxLayout> vbox;
    QtObjPtr<AreYouSureWidget> wgtAreYouSure;
  } consoleAreYouSurePage;

  std::array<QtObjPtr<ConfigPage>, 16> pages;
  QtObjPtr<QLabel> wgtMap;
};

class FConfigMaze : public QWidget, public Fragment {
  Q_OBJECT

  public:
    FConfigMaze(Fragment& parent, FragmentData& parentData, const CommonFragData& commonData);

    virtual void reload(const FragmentSpec& spec) override;
    virtual void cleanUp() override;

    virtual ~FConfigMaze() override;

  private slots:
    void onEnterConsoleClick();
    void onExitConsoleClick();
    void onAreYouSureFinish(bool passed);
    void onPageNextClick(int pageIdx);

  private:
    FConfigMazeData m_data;
    int m_layoutIdxOfConsoleLaunchPage;
    int m_layoutIdxOfAreYouSurePage;
    int m_layoutIdxOfConsolePage;
    int m_layoutIdxOfFirstConfigPage;

    void constructConsoleLaunchPage();
    void constructAreYouSurePage();
    void constructConsolePage();
};


#endif
