#ifndef __PROCALC_EVASIVE_BUTTON_HPP__
#define __PROCALC_EVASIVE_BUTTON_HPP__


#include <QPushButton>


class EvasiveButton : public QPushButton {
  public:
    EvasiveButton(const QString& caption, QWidget* parent);

    ~EvasiveButton() override;
};


#endif
