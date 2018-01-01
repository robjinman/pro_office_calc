#ifndef __PROCALC_FRAGMENTS_F_CONFIG_MAZE_HPP__
#define __PROCALC_FRAGMENTS_F_CONFIG_MAZE_HPP__


#include <memory>
#include <array>
#include <QGridLayout>
#include <QPushButton>
#include <QLabel>
#include <QStackedLayout>
#include "fragments/f_main/f_settings_dialog/f_config_maze/are_you_sure_widget.hpp"
#include "fragments/f_main/f_settings_dialog/f_config_maze/console_widget.hpp"
#include "fragments/f_main/f_settings_dialog/f_config_maze/config_page.hpp"
#include "fragment.hpp"


struct FConfigMazeData : public FragmentData {
  std::unique_ptr<QStackedLayout> stackedLayout;

  struct {
    std::unique_ptr<QWidget> widget;
    std::unique_ptr<QVBoxLayout> vbox;
    std::unique_ptr<ConsoleWidget> wgtConsole;
    std::unique_ptr<QPushButton> wgtBack;
  } consolePage;

  struct {
    std::unique_ptr<QWidget> widget;
    std::unique_ptr<QVBoxLayout> vbox;
    std::unique_ptr<QPushButton> wgtToConsole;
  } consoleLaunchPage;

  struct {
    std::unique_ptr<QWidget> widget;
    std::unique_ptr<QVBoxLayout> vbox;
    std::unique_ptr<AreYouSureWidget> wgtAreYouSure;
  } consoleAreYouSurePage;

  std::array<std::unique_ptr<ConfigPage>, 16> pages;
  std::unique_ptr<QLabel> wgtMap;
};

class FConfigMaze : public QWidget, public Fragment {
  Q_OBJECT

  public:
    FConfigMaze(Fragment& parent, FragmentData& parentData);

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
