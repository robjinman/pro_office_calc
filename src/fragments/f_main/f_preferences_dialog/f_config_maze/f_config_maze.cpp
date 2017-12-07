#include "fragments/f_main/f_preferences_dialog/f_preferences_dialog.hpp"
#include "fragments/f_main/f_preferences_dialog/f_config_maze/f_config_maze.hpp"
#include "fragments/f_main/f_preferences_dialog/f_config_maze/f_config_maze_spec.hpp"


//===========================================
// FConfigMaze::FConfigMaze
//===========================================
FConfigMaze::FConfigMaze(Fragment& parent_, FragmentData& parentData_)
  : QWidget(nullptr),
    Fragment("FConfigMaze", parent_, parentData_, m_data) {}

//===========================================
// FConfigMaze::rebuild
//===========================================
void FConfigMaze::rebuild(const FragmentSpec& spec_) {
  auto& parent = parentFrag<FPreferencesDialog>();
  auto& parentData = parentFragData<FPreferencesDialogData>();

  parentData.vbox->addWidget(this);

  auto& spec = dynamic_cast<const FConfigMazeSpec&>(spec_);

  m_data.wgtButton.reset(new EvasiveButton("Click me!", this));

  Fragment::rebuild(spec_);
}

//===========================================
// FConfigMaze::cleanUp
//===========================================
void FConfigMaze::cleanUp() {
  auto& parentData = parentFragData<FPreferencesDialogData>();

  parentData.vbox->removeWidget(this);
}
