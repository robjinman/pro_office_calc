#ifndef __PROCALC_SHUFFLED_KEYS_VIEW_HPP__
#define __PROCALC_SHUFFLED_KEYS_VIEW_HPP__


class QWidget;
class QLineEdit;

class ShuffledKeysView : public View {
  public:
    ShuffledKeysView(QWidget& widget, QLineEdit& wgtDigitDisplay)

    virtual void setup(int rootState);
};


#endif
