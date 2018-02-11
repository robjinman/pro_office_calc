#ifndef __PROCALC_FRAGMENTS_WIDGET_FLAG_DATA_HPP__
#define __PROCALC_FRAGMENTS_WIDGET_FLAG_DATA_HPP__


#include <QBoxLayout>
#include "fragment.hpp"


struct WidgetFragData : public FragmentData {
  WidgetFragData(QBoxLayout* box)
    : FragmentData(),
      box(box) {}

  QBoxLayout* box;

  virtual ~WidgetFragData() override {}
};


#endif
