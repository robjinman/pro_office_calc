#ifndef __PROCALC_APP_STATE_HPP__
#define __PROCALC_APP_STATE_HPP__


#include <istream>
#include <ostream>
#include <memory>


struct AppState {
  void deserialize(std::istream& is) {
    shallowDeserialize(is);
    postShallowDeserialize();
    deepDeserialize(is);
  }

  virtual void serialize(std::ostream& os) const = 0;
  virtual void shallowDeserialize(std::istream& is) = 0;
  virtual void deepDeserialize(std::istream& is) {}
  virtual void postShallowDeserialize() {}

  virtual ~AppState() {}
};

typedef std::unique_ptr<AppState> pAppState_t;


#endif
