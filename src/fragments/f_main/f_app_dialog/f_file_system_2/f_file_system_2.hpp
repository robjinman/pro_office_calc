#ifndef __PROCALC_FRAGMENTS_F_FILE_SYSTEM_2_HPP__
#define __PROCALC_FRAGMENTS_F_FILE_SYSTEM_2_HPP__


#include <QWidget>
#include <QMargins>
#include <QVBoxLayout>
#include "raycast/raycast_widget.hpp"
#include "fragment.hpp"
#include "qt_obj_ptr.hpp"
#include "event_system.hpp"
#include "fragments/f_main/f_app_dialog/f_file_system_2/game_logic.hpp"


struct FFileSystem2Data : public FragmentData {
  QtObjPtr<QVBoxLayout> vbox;
  QtObjPtr<RaycastWidget> wgtRaycast;
  std::unique_ptr<t_minus_two_minutes::GameLogic> gameLogic;
};

class FFileSystem2 : public QWidget, public Fragment {
  Q_OBJECT

  public:
    FFileSystem2(Fragment& parent, FragmentData& parentData, const CommonFragData& commonData);

    virtual void reload(const FragmentSpec& spec) override;
    virtual void cleanUp() override;

    virtual ~FFileSystem2() override;

  private:
    FFileSystem2Data m_data;

    struct {
      int spacing;
      QMargins margins;
    } m_origParentState;

    EventHandle m_hDialogClosed;
};


#endif
