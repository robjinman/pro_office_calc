#include <QMouseEvent>
#include "fragments/f_main/f_settings_dialog/f_settings_dialog.hpp"
#include "fragments/f_main/f_settings_dialog/f_config_maze/f_config_maze.hpp"
#include "fragments/f_main/f_settings_dialog/f_config_maze/f_config_maze_spec.hpp"
#include "fragments/f_main/f_settings_dialog/f_config_maze/f_are_you_sure/f_are_you_sure_spec.hpp"
#include "utils.hpp"



struct ContentSpec : public FragmentSpec {
  ContentSpec()
    : FragmentSpec("FConfigMaze", {
        &areYouSureSpec
      }) {}

  FAreYouSureSpec areYouSureSpec;
};


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
  auto& parent = parentFrag<FSettingsDialog>();
  auto& parentData = parentFragData<FSettingsDialogData>();

  m_data.eventSystem = parentData.eventSystem;
  m_data.updateLoop = parentData.updateLoop;

  setMouseTracking(true);

  m_data.wgtButton.reset(new QPushButton("Admin console"));

  m_data.vbox.reset(new QVBoxLayout);
  m_data.vbox->addWidget(m_data.wgtButton.get());

  connect(m_data.wgtButton.get(), SIGNAL(pressed()), this, SLOT(onBtnClick()));

  auto& spec = dynamic_cast<const FConfigMazeSpec&>(spec_);

  setLayout(m_data.vbox.get());

  parentData.vbox->addWidget(this);

  Fragment::rebuild(spec_);
}

//===========================================
// FConfigMaze::onBtnClick
//===========================================
void FConfigMaze::onBtnClick() {
  DBG_PRINT("Button clicked!\n");

  ContentSpec spec;
  spec.areYouSureSpec.setEnabled(true);

  Fragment::rebuild(spec);
}

//===========================================
// FConfigMaze::cleanUp
//===========================================
void FConfigMaze::cleanUp() {
  auto& parentData = parentFragData<FSettingsDialogData>();

  parentData.vbox->removeWidget(this);
}
