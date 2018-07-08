#ifndef __PROCALC_FRAGMENTS_F_DOOMSWEEPER_HPP__
#define __PROCALC_FRAGMENTS_F_DOOMSWEEPER_HPP__


#include <QWidget>
#include <QMargins>
#include <QVBoxLayout>
#include "raycast/raycast_widget.hpp"
#include "fragment.hpp"
#include "qt_obj_ptr.hpp"
#include "event_system.hpp"
#include "fragments/f_main/f_app_dialog/f_doomsweeper/game_logic.hpp"


struct FDoomsweeperData : public FragmentData {
  QtObjPtr<QVBoxLayout> vbox;
  QtObjPtr<RaycastWidget> wgtRaycast;
  std::unique_ptr<doomsweeper::GameLogic> gameLogic;
};

class FDoomsweeper : public QWidget, public Fragment {
  Q_OBJECT

  public:
    FDoomsweeper(Fragment& parent, FragmentData& parentData, const CommonFragData& commonData);

    virtual void reload(const FragmentSpec& spec) override;
    virtual void cleanUp() override;

    virtual ~FDoomsweeper() override;

  private:
    bool waitForInit();

    FDoomsweeperData m_data;

    struct {
      int spacing;
      QMargins margins;
    } m_origParentState;

    std::future<void> m_initFuture;
    EventHandle m_hSetup;
};


#endif
