#ifndef __PROCALC_FRAGMENTS_F_SERVER_ROOM_HPP__
#define __PROCALC_FRAGMENTS_F_SERVER_ROOM_HPP__


#include <QWidget>
#include <QMargins>
#include <QVBoxLayout>
#include "raycast/raycast_widget.hpp"
#include "fragment.hpp"
#include "qt_obj_ptr.hpp"


struct FServerRoomData : public FragmentData {
  QtObjPtr<QVBoxLayout> vbox;
  QtObjPtr<RaycastWidget> wgtRaycast;
};

class FServerRoom : public QWidget, public Fragment {
  Q_OBJECT

  public:
    FServerRoom(Fragment& parent, FragmentData& parentData, const CommonFragData& commonData);

    virtual void reload(const FragmentSpec& spec) override;
    virtual void cleanUp() override;

    virtual ~FServerRoom() override;

  private:
    FServerRoomData m_data;

    struct {
      int spacing;
      QMargins margins;
    } m_origParentState;

    int m_launchEventId = -1;
};


#endif
