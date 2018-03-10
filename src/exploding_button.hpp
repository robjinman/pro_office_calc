#ifndef __PROCALC_EXPLODING_BUTTON_HPP__
#define __PROCALC_EXPLODING_BUTTON_HPP__


#include <QPushButton>
#include "qt_obj_ptr.hpp"


class ExplodingButton : public QPushButton {
  Q_OBJECT

  public:
    ExplodingButton(const QString& caption);

    ~ExplodingButton() override;

  private:

};


#endif
