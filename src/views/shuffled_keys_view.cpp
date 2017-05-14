#include <QLineEdit>
#include "shuffled_keys_view.hpp"
#include "effects.hpp"
#include "exception.hpp"


ShuffledKeysView::ShuffledKeysView(QWidget& widget, QLineEdit& wgtDigitDisplay)
  : m_widget(widget), m_wgtDigitDisplay(wgtDigitDisplay) {}

void ShuffledKeysView::setup(int rootState) {
  if (rootState != ST_SHUFFLED_KEYS) {
    EXCEPTION("ShuffledKeysView cannot be setup for rootState " << rootState);
  }

  m_widget.setColour(*this, QColor(160, 160, 160), QPalette::Window);
  setColour(m_wgtDigitDisplay, QColor(160, 120, 120), QPalette::Base);
}
