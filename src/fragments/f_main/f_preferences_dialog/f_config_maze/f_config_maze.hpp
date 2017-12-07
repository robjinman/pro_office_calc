#ifndef __PROCALC_FRAGMENTS_F_CONFIG_MAZE_HPP__
#define __PROCALC_FRAGMENTS_F_CONFIG_MAZE_HPP__


#include <memory>
#include <QWidget>
#include "fragment.hpp"
#include "evasive_button.hpp"


struct FConfigMazeData : public FragmentData {
  std::unique_ptr<EvasiveButton> wgtButton;
};

class FConfigMaze : public QWidget, public Fragment {
  Q_OBJECT

  public:
    FConfigMaze(Fragment& parent, FragmentData& parentData);

    virtual void rebuild(const FragmentSpec& spec) override;
    virtual void cleanUp() override;

  private:
    FConfigMazeData m_data;
};


#endif
