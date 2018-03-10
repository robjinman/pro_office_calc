#ifndef __PROCALC_EXPLODING_BUTTON_HPP__
#define __PROCALC_EXPLODING_BUTTON_HPP__


#include <QPushButton>
#include <QLabel>
#include <QImage>
#include "qt_obj_ptr.hpp"


class UpdateLoop;

class ExplodingButton : public QPushButton {
  Q_OBJECT

  public:
    ExplodingButton(QWidget* parent, const QString& caption, UpdateLoop& updateLoop);

    ~ExplodingButton() override;

  private slots:
    void onClick();

  private:
    UpdateLoop& m_updateLoop;
    QtObjPtr<QLabel> m_wgtLabel;
    std::unique_ptr<QImage> m_spriteSheet;
};


#endif
