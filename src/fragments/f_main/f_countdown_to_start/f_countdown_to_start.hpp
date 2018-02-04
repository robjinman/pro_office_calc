#ifndef __PROCALC_FRAGMENTS_F_COUNTDOWN_TO_START_HPP__
#define __PROCALC_FRAGMENTS_F_COUNTDOWN_TO_START_HPP__


#include <functional>
#include "fragment.hpp"


struct FCountdownToStartData : public FragmentData {};

class FCountdownToStart : public Fragment {
  public:
    FCountdownToStart(Fragment& parent, FragmentData& parentData, const CommonFragData& commonData);

    virtual void reload(const FragmentSpec& spec) override;
    virtual void cleanUp() override;

    virtual ~FCountdownToStart() override;

  private:
    void onQuit();

    struct {
      std::function<void()> fnOnQuit;
    } m_origParentState;

    FCountdownToStartData m_data;
    int m_stateId;
};


#endif
