#ifndef __PROCALC_EVENT_SYSTEM_HPP__
#define __PROCALC_EVENT_SYSTEM_HPP__


#include <functional>
#include <map>
#include <set>
#include <QObject>
#include "event.hpp"


typedef std::function<void(const Event&)> handlerFunc_t;


class EventSystem : public QObject {
  Q_OBJECT

  public:
    void listen(const std::string& name, handlerFunc_t fn, int& id);
    void forget(int id);
    void fire(pEvent_t event);

  private:
    bool event(QEvent* event) override;
    void processEvent(const Event& event);
    void processEvent_(const std::string& name, const Event& event);
    void forget_(int id);
    void processingStart();
    void processingEnd();
    void forgetPending();
    void addPending();

    std::map<std::string, std::map<int, handlerFunc_t>> m_handlers;

    bool m_processingEvent = false;
    std::map<std::string, std::map<int, handlerFunc_t>> m_pendingAddition;
    std::set<int> m_pendingForget;
};


#endif
