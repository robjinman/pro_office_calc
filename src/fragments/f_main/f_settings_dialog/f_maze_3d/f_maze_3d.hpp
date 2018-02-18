#ifndef __PROCALC_FRAGMENTS_F_MAZE_3D_HPP__
#define __PROCALC_FRAGMENTS_F_MAZE_3D_HPP__


#include <QWidget>
#include <QMargins>
#include <QVBoxLayout>
#include "raycast/raycast_widget.hpp"
#include "fragment.hpp"
#include "qt_obj_ptr.hpp"


struct FMaze3dData : public FragmentData {
  QtObjPtr<QVBoxLayout> vbox;
  QtObjPtr<RaycastWidget> wgtRaycast;
};

class FMaze3d : public QWidget, public Fragment {
  Q_OBJECT

  public:
    FMaze3d(Fragment& parent, FragmentData& parentData, const CommonFragData& commonData);

    virtual void reload(const FragmentSpec& spec) override;
    virtual void cleanUp() override;

    virtual ~FMaze3d() override;

  private:
    FMaze3dData m_data;

    struct {
      int spacing;
      QMargins margins;
    } m_origParentState;
};


#endif
