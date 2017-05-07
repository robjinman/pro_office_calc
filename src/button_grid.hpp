#ifndef __PROCALC_BUTTON_GRID_HPP__
#define __PROCALC_BUTTON_GRID_HPP__


#include <memory>
#include <vector>
#include <QWidget>


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
  BTN_EQUALS
};


class QGridLayout;
class QButtonGroup;
class QPushButton;
class AppState;
class EventSystem;
class UpdateLoop;

class ButtonGrid : public QWidget {
  Q_OBJECT

  public:
    ButtonGrid(AppState& appState, EventSystem& eventSystem,
      UpdateLoop& updateLoop, QWidget* parent);

    virtual ~ButtonGrid();

  private slots:
    void onBtnClick(int id);

  signals:
    void buttonClicked(int id);

  private:
    AppState& m_appState;
    EventSystem& m_eventSystem;
    UpdateLoop& m_updateLoop;
    std::vector<std::unique_ptr<QPushButton>> m_buttons;
    std::unique_ptr<QButtonGroup> m_grpButtons;
};


#endif
