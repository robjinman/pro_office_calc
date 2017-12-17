#ifndef __PROCALC_FRAGMENTS_F_CONFIG_MAZE_HPP__
#define __PROCALC_FRAGMENTS_F_CONFIG_MAZE_HPP__


#include <memory>
#include <QGridLayout>
#include <QPushButton>
#include "fragments/f_main/f_settings_dialog/f_config_maze/are_you_sure_widget.hpp"
#include "fragment.hpp"


class EventSystem;
class UpdateLoop;

struct FConfigMazeData : public FragmentData {
  EventSystem* eventSystem;
  UpdateLoop* updateLoop;
  std::unique_ptr<QGridLayout> grid;
  std::unique_ptr<QPushButton> wgtButton;
  std::unique_ptr<AreYouSureWidget> wgtAreYouSure;
};

class FConfigMaze : public QWidget, public Fragment {
  Q_OBJECT

  public:
    FConfigMaze(Fragment& parent, FragmentData& parentData);

    virtual void rebuild(const FragmentSpec& spec) override;
    virtual void cleanUp() override;

    virtual ~FConfigMaze() override;

  private slots:
    void onBtnClick();

  private:
    void showAreYouSure();
    void showContinueToAdminConsole();
    void onAreYouSurePass();
    void onAreYouSureFail();

    FConfigMazeData m_data;
    int m_areYouSurePassId = -1;
    int m_areYouSureFailId = -1;
};


#endif
