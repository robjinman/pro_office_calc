#ifndef __PROCALC_MAIN_STATE_HPP__
#define __PROCALC_MAIN_STATE_HPP__


#include <string>
#include <fstream>
#include "app_state.hpp"
#include "utils.hpp"


struct MainSub0State : public AppState {
  int openCount = 11;

  virtual void serialize(std::ostream& os) const override {
    os.write(reinterpret_cast<const char*>(&openCount), sizeof(openCount));
  }

  virtual void shallowDeserialize(std::istream& is) override {
    is.read(reinterpret_cast<char*>(&openCount), sizeof(openCount));
  }
};

struct MainSub1State : public AppState {
  bool divByZero = false;

  virtual void serialize(std::ostream& os) const override {
    os.write(reinterpret_cast<const char*>(&divByZero), sizeof(divByZero));
  }

  virtual void shallowDeserialize(std::istream& is) override {
    is.read(reinterpret_cast<char*>(&divByZero), sizeof(divByZero));
  }
};

struct MainSub2State : public AppState {
  int a;

  virtual void serialize(std::ostream& os) const override {
    os.write(reinterpret_cast<const char*>(&a), sizeof(a));
  }

  virtual void shallowDeserialize(std::istream& is) override {
    is.read(reinterpret_cast<char*>(&a), sizeof(a));
  }
};

struct MainState : public AppState {
  enum LEVEL {
    LVL_NORMAL_CALCULATOR,
    LVL_DANGER_INFINITY,
    LVL_WEIRD
  };

  int level = LVL_NORMAL_CALCULATOR;
  std::string msg1 = "hello";
  std::string msg2 = "world";
  float f = 1.23;
  pAppState_t subState = pAppState_t(new MainSub0State);

  virtual void serialize(std::ostream& os) const override {
    os.write(reinterpret_cast<const char*>(&level), sizeof(level));
    writeString(os, msg1);
    writeString(os, msg2);
    os.write(reinterpret_cast<const char*>(&f), sizeof(f));

    subState->serialize(os);
  }

  virtual void shallowDeserialize(std::istream& is) override {
    is.read(reinterpret_cast<char*>(&level), sizeof(level));
    msg1 = readString(is);
    msg2 = readString(is);
    is.read(reinterpret_cast<char*>(&f), sizeof(f));
  }

  virtual void deepDeserialize(std::istream& is) override {
    subState->deserialize(is);
  }

  virtual void postShallowDeserialize() override {
    switch (level) {
      case LVL_NORMAL_CALCULATOR:
        subState.reset(new MainSub0State);
        break;
      case LVL_DANGER_INFINITY:
        subState.reset(new MainSub1State);
        break;
      case LVL_WEIRD:
        subState.reset(new MainSub2State);
        break;
    }
  }
};


#endif
