#ifndef __PROCALC_FRAGMENTS_F_FILE_SYSTEM_HPP__
#define __PROCALC_FRAGMENTS_F_FILE_SYSTEM_HPP__


#include <QWidget>
#include <QMargins>
#include <QVBoxLayout>
#include "raycast/raycast_widget.hpp"
#include "fragment.hpp"
#include "qt_obj_ptr.hpp"
#include "fragments/f_main/f_app_dialog/f_file_system/game_logic.hpp"


struct FFileSystemData : public FragmentData {
  QtObjPtr<QVBoxLayout> vbox;
  QtObjPtr<RaycastWidget> wgtRaycast;
  std::unique_ptr<going_in_circles::GameLogic> gameLogic;
};

class FFileSystem : public QWidget, public Fragment {
  Q_OBJECT

  public:
    FFileSystem(Fragment& parent, FragmentData& parentData, const CommonFragData& commonData);

    virtual void reload(const FragmentSpec& spec) override;
    virtual void cleanUp() override;

    virtual ~FFileSystem() override;

  private:
    FFileSystemData m_data;

    struct {
      int spacing;
      QMargins margins;
    } m_origParentState;
};


#endif
