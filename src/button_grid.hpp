#ifndef __PROCALC_BUTTON_GRID_HPP__
#define __PROCALC_BUTTON_GRID_HPP__


#include <memory>
#include <vector>
#include <QWidget>


class QGridLayout;
class QButtonGroup;
class QPushButton;

class ButtonGrid : public QWidget {
  Q_OBJECT

  public:
    ButtonGrid(QWidget* parent = 0);

    virtual ~ButtonGrid();

  private slots:
    void onBtnClick(int id);

  signals:
    void buttonClicked(int id);

  private:
    std::vector<std::unique_ptr<QPushButton>> m_buttons;
    std::unique_ptr<QButtonGroup> m_grpButtons;
};


#endif
