#ifndef __PROCALC_FRAGMENTS_F_CONFIG_MAZE_CONFIG_PAGE_HPP__
#define __PROCALC_FRAGMENTS_F_CONFIG_MAZE_CONFIG_PAGE_HPP__


#include <vector>
#include <list>
#include <QWidget>
#include <QRadioButton>
#include <QGridLayout>
#include <QPushButton>
#include <QButtonGroup>
#include <QLabel>
#include "qt_obj_ptr.hpp"


class ConfigPage : public QWidget {
  Q_OBJECT

  public:
    ConfigPage(QChar symbol, std::vector<int> neighbours = {});

    void reset();

    QtObjPtr<QGridLayout> grid;

  signals:
    void nextClicked(int pageIdx);

  private slots:
    void onNextClick();

  private:
    std::vector<int> m_neighbours;
    QtObjPtr<QLabel> m_label;
    std::list<QtObjPtr<QRadioButton>> m_radioBtns;
    QtObjPtr<QButtonGroup> m_btnGroup;
    QtObjPtr<QPushButton> m_wgtNext;
};


#endif
