#ifndef __PROCALC_BUTTON_GRID_HPP__
#define __PROCALC_BUTTON_GRID_HPP__


#include <memory>
#include <vector>
#include <QWidget>
#include <QGridLayout>
#include "qt_obj_ptr.hpp"


enum buttonId_t {
  BTN_ZERO = 0,
  BTN_ONE = 1,
  BTN_TWO = 2,
  BTN_THREE = 3,
  BTN_FOUR = 4,
  BTN_FIVE = 5,
  BTN_SIX = 6,
  BTN_SEVEN = 7,
  BTN_EIGHT = 8,
  BTN_NINE = 9,
  BTN_PLUS,
  BTN_MINUS,
  BTN_TIMES,
  BTN_DIVIDE,
  BTN_POINT,
  BTN_CLEAR,
  BTN_EQUALS,

  BTN_NULL = 1000
};


class QGridLayout;
class QButtonGroup;
class QPushButton;

class ButtonGrid : public QWidget {
  Q_OBJECT

  public:
    ButtonGrid(QWidget* parent);
    virtual ~ButtonGrid();

    QtObjPtr<QGridLayout> grid;
    std::vector<QtObjPtr<QPushButton>> buttons;
    QtObjPtr<QButtonGroup> buttonGroup;

  private slots:
    void onBtnClick(int id);
    void onBtnPress(int id);

  signals:
    void buttonClicked(int id);
    void buttonPressed(int id);
};


#endif
