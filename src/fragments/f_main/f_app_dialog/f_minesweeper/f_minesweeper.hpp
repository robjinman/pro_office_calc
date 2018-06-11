#ifndef __PROCALC_FRAGMENTS_F_MINESWEEPER_HPP__
#define __PROCALC_FRAGMENTS_F_MINESWEEPER_HPP__


#include <array>
#include <QWidget>
#include <QGridLayout>
#include <QPushButton>
#include <QButtonGroup>
#include "fragment.hpp"
#include "qt_obj_ptr.hpp"


struct FMinesweeperData : public FragmentData {
  QtObjPtr<QGridLayout> grid;
  std::array<std::array<QtObjPtr<QPushButton>, 10>, 10> buttons;
  QtObjPtr<QButtonGroup> buttonGroup;
};

class FMinesweeper : public QWidget, public Fragment {
  Q_OBJECT

  public:
    FMinesweeper(Fragment& parent, FragmentData& parentData, const CommonFragData& commonData);

    virtual void reload(const FragmentSpec& spec) override;
    virtual void cleanUp() override;

    virtual ~FMinesweeper() override;

  private slots:
    void onBtnClick(int id);

  private:
    FMinesweeperData m_data;
};


#endif
