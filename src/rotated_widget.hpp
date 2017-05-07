#ifndef __PROCALC_ROTATED_WIDGET_HPP__
#define __PROCALC_ROTATED_WIDGET_HPP__


#include <QWidget>


class QPaintEvent;

class RotatedWidget : public QWidget {
  Q_OBJECT

  public:
    RotatedWidget(QWidget* parent = nullptr);

    void setBaseWidget(QWidget* baseWidget);
    QWidget* baseWidget() const;

  protected:
    virtual void paintEvent(QPaintEvent* event) override;

  private:
    QWidget* m_baseWidget;
};


#endif
