#ifndef __PROCALC_APPLICATION_HPP__
#define __PROCALC_APPLICATION_HPP__


#include <QApplication>


class Application : public QApplication {
  public:
    Application(int argc, char** argv)
      : QApplication(argc, argv) {}

    virtual bool notify(QObject* receiver, QEvent* event);
};


#endif
