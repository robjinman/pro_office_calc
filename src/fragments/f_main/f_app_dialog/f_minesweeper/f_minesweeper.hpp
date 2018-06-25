#ifndef __PROCALC_FRAGMENTS_F_MINESWEEPER_HPP__
#define __PROCALC_FRAGMENTS_F_MINESWEEPER_HPP__


#include <array>
#include <set>
#include <QWidget>
#include <QGridLayout>
#include <QPushButton>
#include <QLabel>
#include <QButtonGroup>
#include <QStackedLayout>
#include <QMouseEvent>
#include "fragment.hpp"
#include "qt_obj_ptr.hpp"
#include "fragments/f_main/f_app_dialog/f_minesweeper/events.hpp"


class GoodButton : public QPushButton {
  Q_OBJECT

  public:
    GoodButton()
      : QPushButton() {}

  signals:
    void rightClicked(QAbstractButton*);

  protected:
    void mousePressEvent(QMouseEvent* event) override {
      if (event->button() == Qt::RightButton) {
        emit rightClicked(this);
      }

      QPushButton::mousePressEvent(event);
    }
};

class GoodButtonGroup : public QButtonGroup {
  Q_OBJECT

  public:
    GoodButtonGroup()
      : QButtonGroup() {}

    void addGoodButton(GoodButton* button, int id = -1) {
      QButtonGroup::addButton(button, id);

      connect(button, SIGNAL(rightClicked(QAbstractButton*)), this,
        SLOT(onRightClick(QAbstractButton*)));
    }

  signals:
    void rightClicked(int);

  private slots:
    void onRightClick(QAbstractButton* btn) {
      emit rightClicked(id(btn));
    }
};

struct IconSet {
  QIcon mine;
  QIcon noMine;
  QIcon flag;
  QIcon player;
};

class MinesweeperCell : public QWidget {
  Q_OBJECT

  public:
    MinesweeperCell(int row, int col, const IconSet& icons);

    const int row;
    const int col;

    int value() const {
      return m_value;
    }

    void setValue(int val);

    GoodButton& button() const {
      return *m_button;
    }

    bool hidden() const;
    void setHidden(bool hidden);

    bool flagged() const {
      return m_flagged;
    }

    void setFlagged(bool flagged);

    void onPlayerChangeCell(int row, int col);
    void onPlayerClick();

  private:
    void render();

    const IconSet& m_icons;

    int m_value;
    bool m_flagged = false;
    bool m_exploded = false;

    QPixmap m_pixmap;

    QtObjPtr<QStackedLayout> m_stackedLayout;
    QtObjPtr<QLabel> m_label;
    QtObjPtr<GoodButton> m_button;

    bool m_hasPlayer = false;
};

struct FMinesweeperData : public FragmentData {};

class FMinesweeper : public QWidget, public Fragment {
  Q_OBJECT

  public:
    FMinesweeper(Fragment& parent, FragmentData& parentData, const CommonFragData& commonData);

    virtual void reload(const FragmentSpec& spec) override;
    virtual void cleanUp() override;

    virtual ~FMinesweeper() override;

  private slots:
    void onBtnClick(int id);
    void onBtnRightClick(int id);

  private:
    std::set<doomsweeper::Coord> placeMines();
    void setNumbers();
    std::set<MinesweeperCell*> getNeighbours(const MinesweeperCell& cell) const;
    void clearNeighbours_r(const MinesweeperCell& cell, std::set<const MinesweeperCell*>& visited);
    void onInnerCellEntered(const Event& e_);

    FMinesweeperData m_data;
    QtObjPtr<QGridLayout> m_grid;
    std::array<std::array<QtObjPtr<MinesweeperCell>, 8>, 8> m_cells;
    QtObjPtr<GoodButtonGroup> m_buttonGroup;
    IconSet m_icons;
    int m_innerCellEnteredIdx = -1;
};


#endif
