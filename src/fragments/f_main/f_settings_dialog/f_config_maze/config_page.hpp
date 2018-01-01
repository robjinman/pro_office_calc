#ifndef __PROCALC_FRAGMENTS_F_CONFIG_MAZE_CONFIG_PAGE_HPP__
#define __PROCALC_FRAGMENTS_F_CONFIG_MAZE_CONFIG_PAGE_HPP__


#include <initializer_list>
#include <vector>
#include <list>
#include <memory>
#include <QWidget>
#include <QRadioButton>
#include <QGridLayout>
#include <QPushButton>
#include <QButtonGroup>
#include <QLabel>


class ConfigPage : public QWidget {
  Q_OBJECT

  public:
    ConfigPage(QChar symbol, std::initializer_list<int> neighbours = {});

    void reset();

    std::unique_ptr<QGridLayout> grid;

  signals:
    void nextClicked(int pageIdx);

  private slots:
    void onNextClick();

  private:
    std::vector<int> m_neighbours;
    std::unique_ptr<QLabel> m_label;
    std::list<std::unique_ptr<QRadioButton>> m_radioBtns;
    std::unique_ptr<QButtonGroup> m_btnGroup;
    std::unique_ptr<QPushButton> m_wgtNext;
};


#endif
