#ifndef __PROCALC_FRAGMENTS_F_KERNEL_HPP__
#define __PROCALC_FRAGMENTS_F_KERNEL_HPP__


#include <QWidget>
#include <QMargins>
#include <QVBoxLayout>
#include "raycast/raycast_widget.hpp"
#include "fragment.hpp"
#include "qt_obj_ptr.hpp"
#include "fragments/f_main/f_app_dialog/f_kernel/game_logic.hpp"


struct FKernelData : public FragmentData {
  QtObjPtr<QVBoxLayout> vbox;
  QtObjPtr<RaycastWidget> wgtRaycast;
  std::unique_ptr<millennium_bug::GameLogic> gameLogic;
};

class FKernel : public QWidget, public Fragment {
  Q_OBJECT

  public:
    FKernel(Fragment& parent, FragmentData& parentData, const CommonFragData& commonData);

    virtual void reload(const FragmentSpec& spec) override;
    virtual void cleanUp() override;

    virtual ~FKernel() override;

  private:
    FKernelData m_data;

    struct {
      int spacing;
      QMargins margins;
    } m_origParentState;
};


#endif
