#ifndef __PROCALC_FRAGMENTS_F_CONFIG_MAZE_HPP__
#define __PROCALC_FRAGMENTS_F_CONFIG_MAZE_HPP__


#include <memory>
#include <QWidget>
#include "fragment.hpp"
#include "evasive_button.hpp"


class EventSystem;
class UpdateLoop;

struct FConfigMazeData : public FragmentData {
  EventSystem* eventSystem;
  UpdateLoop* updateLoop;
  std::unique_ptr<EvasiveButton> wgtButton;
};

class FConfigMaze : public QWidget, public Fragment {
  Q_OBJECT

  public:
    FConfigMaze(Fragment& parent, FragmentData& parentData);

    virtual void rebuild(const FragmentSpec& spec) override;
    virtual void cleanUp() override;

  protected:
    void mouseMoveEvent(QMouseEvent* event) override;

  private slots:
    void onBtnClick();

  private:
    FConfigMazeData m_data;
};


#endif
