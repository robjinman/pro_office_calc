#include <algorithm>
#include "fragments/f_main/f_settings_dialog/f_config_maze/config_page.hpp"


//===========================================
// ConfigPage::ConfigPage
//===========================================
ConfigPage::ConfigPage(QChar symbol, std::initializer_list<int> neighbours)
  : QWidget(nullptr),
    m_neighbours(neighbours) {

  std::random_shuffle(m_neighbours.begin(), m_neighbours.end());

  grid.reset(new QGridLayout);
  setLayout(grid.get());

  QFont f = font();
  f.setPointSize(16);

  m_label.reset(new QLabel(symbol));
  m_label->setFont(f);
  grid->addWidget(m_label.get(), 0, 0);

  m_btnGroup.reset(new QButtonGroup);

  QString labels = "ABCD";
  for (unsigned int i = 0; i < m_neighbours.size(); ++i) {
    m_radioBtns.push_back(std::unique_ptr<QRadioButton>(new QRadioButton(QString("Option ")
      + QString(labels[i]))));
    m_btnGroup->addButton(m_radioBtns.back().get(), m_neighbours[i]);
    grid->addWidget(m_radioBtns.back().get(), i + 1, 1);
  }

  m_wgtNext.reset(new QPushButton("Next"));

  grid->addWidget(m_wgtNext.get(), 5, 2);
  connect(m_wgtNext.get(), SIGNAL(clicked()), this, SLOT(onNextClick()));

  reset();
}

//===========================================
// ConfigPage::reset
//===========================================
void ConfigPage::reset() {
  m_radioBtns.front()->setChecked(true);
}

//===========================================
// ConfigPage::onNextClick
//===========================================
void ConfigPage::onNextClick() {
  emit nextClicked(m_btnGroup->checkedId());
}