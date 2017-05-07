#include <string>
#include <QGridLayout>
#include <QButtonGroup>
#include <QPushButton>
#include "button_grid.hpp"
#include "utils.hpp"
#include "rotated_widget.hpp"


using std::unique_ptr;
using std::string;


//===========================================
// makeButton
//===========================================
static QPushButton* makeButton(QWidget* parent, const QString& text) {
  QPushButton* btn = new QPushButton(text, parent);

  btn->setMaximumWidth(60);
  btn->setMaximumHeight(60);

  return btn;
}

//===========================================
// ButtonGrid::ButtonGrid
//===========================================
ButtonGrid::ButtonGrid(QWidget* parent)
  : QWidget(parent) {

  QGridLayout* grid = new QGridLayout;
  grid->setSpacing(1);

  setLayout(grid);

  unique_ptr<QPushButton> btn0(makeButton(this, "0"));
  unique_ptr<QPushButton> btn1(makeButton(this, "1"));
  unique_ptr<QPushButton> btn2(makeButton(this, "2"));
  unique_ptr<QPushButton> btn3(makeButton(this, "3"));
  unique_ptr<QPushButton> btn4(makeButton(this, "4"));
  unique_ptr<QPushButton> btn5(makeButton(this, "5"));
  unique_ptr<QPushButton> btn6(makeButton(this, "6"));
  unique_ptr<QPushButton> btn7(makeButton(this, "7"));
  unique_ptr<QPushButton> btn8(makeButton(this, "8"));
  unique_ptr<QPushButton> btn9(makeButton(this, "9"));
  unique_ptr<QPushButton> btnPlus(makeButton(this, "+"));
  unique_ptr<QPushButton> btnMinus(makeButton(this, "-"));
  unique_ptr<QPushButton> btnTimes(makeButton(this, "*"));
  unique_ptr<QPushButton> btnDivide(makeButton(this, "/"));
  unique_ptr<QPushButton> btnPoint(makeButton(this, "."));
  unique_ptr<QPushButton> btnClear(makeButton(this, "C"));
  unique_ptr<QPushButton> btnEquals(makeButton(this, "="));

  m_grpButtons.reset(new QButtonGroup);
  m_grpButtons->addButton(btn0.get(), BTN_ZERO);
  m_grpButtons->addButton(btn1.get(), BTN_ONE);
  m_grpButtons->addButton(btn2.get(), BTN_TWO);
  m_grpButtons->addButton(btn3.get(), BTN_THREE);
  m_grpButtons->addButton(btn4.get(), BTN_FOUR);
  m_grpButtons->addButton(btn5.get(), BTN_FIVE);
  m_grpButtons->addButton(btn6.get(), BTN_SIX);
  m_grpButtons->addButton(btn7.get(), BTN_SEVEN);
  m_grpButtons->addButton(btn8.get(), BTN_EIGHT);
  m_grpButtons->addButton(btn9.get(), BTN_NINE);
  m_grpButtons->addButton(btnPlus.get(), BTN_PLUS);
  m_grpButtons->addButton(btnMinus.get(), BTN_MINUS);
  m_grpButtons->addButton(btnTimes.get(), BTN_TIMES);
  m_grpButtons->addButton(btnDivide.get(), BTN_DIVIDE);
  m_grpButtons->addButton(btnPoint.get(), BTN_POINT);
  m_grpButtons->addButton(btnClear.get(), BTN_CLEAR);
  m_grpButtons->addButton(btnEquals.get(), BTN_EQUALS);

  grid->addWidget(btnClear.get(), 0, 0);
  grid->addWidget(btn0.get(), 4, 0);
  grid->addWidget(btnPoint.get(), 4, 1);
  grid->addWidget(btnEquals.get(), 4, 2);
  grid->addWidget(btn1.get(), 3, 0);
  grid->addWidget(btn2.get(), 3, 1);
  grid->addWidget(btn3.get(), 3, 2);
  grid->addWidget(btn4.get(), 2, 0);
  grid->addWidget(btn5.get(), 2, 1);
  grid->addWidget(btn6.get(), 2, 2);
  grid->addWidget(btn7.get(), 1, 0);
  grid->addWidget(btn8.get(), 1, 1);
  grid->addWidget(btn9.get(), 1, 2);
  grid->addWidget(btnPlus.get(), 1, 3);
  grid->addWidget(btnMinus.get(), 2, 3);
  grid->addWidget(btnTimes.get(), 3, 3);
  grid->addWidget(btnDivide.get(), 4, 3);

  m_buttons.push_back(std::move(btn0));
  m_buttons.push_back(std::move(btn1));
  m_buttons.push_back(std::move(btn2));
  m_buttons.push_back(std::move(btn3));
  m_buttons.push_back(std::move(btn4));
  m_buttons.push_back(std::move(btn5));
  m_buttons.push_back(std::move(btn6));
  m_buttons.push_back(std::move(btn7));
  m_buttons.push_back(std::move(btn8));
  m_buttons.push_back(std::move(btn9));
  m_buttons.push_back(std::move(btnPlus));
  m_buttons.push_back(std::move(btnMinus));
  m_buttons.push_back(std::move(btnTimes));
  m_buttons.push_back(std::move(btnDivide));
  m_buttons.push_back(std::move(btnPoint));
  m_buttons.push_back(std::move(btnClear));
  m_buttons.push_back(std::move(btnEquals));

  connect(m_grpButtons.get(), SIGNAL(buttonClicked(int)), this, SLOT(onBtnClick(int)));
}

//===========================================
// ButtonGrid::onBtnClick
//===========================================
void ButtonGrid::onBtnClick(int id) {
  emit buttonClicked(id);
}

//===========================================
// ButtonGrid::~ButtonGrid
//===========================================
ButtonGrid::~ButtonGrid() {
  DBG_PRINT("ButtonGrid::~ButtonGrid()\n");
}
