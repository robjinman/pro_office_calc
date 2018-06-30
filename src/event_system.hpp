#ifndef __PROCALC_EVENT_SYSTEM_HPP__
#define __PROCALC_EVENT_SYSTEM_HPP__


#include <functional>
#include <map>
#include <set>
#include <QObject>
#include "event.hpp"


typedef std::function<void(const Event&)> handlerFunc_t;

class EventSystem : public QObject, public std::enable_shared_from_this<EventSystem> {
  Q_OBJECT

  public:
    class Handle {
      public:
        Handle()
          : id(-1) {}

        Handle(std::weak_ptr<EventSystem> eventSystem, int id)
          : id(id),
            m_eventSystem(eventSystem) {}

        Handle(const Handle& cpy) = delete;
        Handle& operator=(const Handle& rhs) = delete;

        Handle(Handle&& cpy) {
          *this = std::move(cpy);
        }

        void forget() {
          auto es = m_eventSystem.lock();
          if (es) {
            es->forget(*this);
          }
        }

        Handle& operator=(Handle&& rhs) {
          if (id != rhs.id) {
            forget();
          }

          id = rhs.id;
          m_eventSystem = rhs.m_eventSystem;

          rhs.id = -1;

          return *this;
        }

        int id;

        ~Handle() {
          forget();
        }

      private:
        std::weak_ptr<EventSystem> m_eventSystem;
    };

    Handle listen(const std::string& name, handlerFunc_t fn);
    void forget(Handle& handle);
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

typedef EventSystem::Handle EventHandle;


#endif
