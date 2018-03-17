#ifndef __PROCALC_FRAGMENTS_WIDGET_FLAG_DATA_HPP__
#define __PROCALC_FRAGMENTS_WIDGET_FLAG_DATA_HPP__


#include <QBoxLayout>
#include "fragment.hpp"
#include "qt_obj_ptr.hpp"


struct WidgetFragData : public FragmentData {
  WidgetFragData(QtObjPtr<QBoxLayout> box)
    : FragmentData(),
      box(std::move(box)) {}

  QtObjPtr<QBoxLayout> box;

  virtual ~WidgetFragData() override {}
};


#endif
