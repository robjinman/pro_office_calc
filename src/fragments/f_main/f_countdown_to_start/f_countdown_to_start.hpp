#ifndef __PROCALC_FRAGMENTS_F_COUNTDOWN_TO_START_HPP__
#define __PROCALC_FRAGMENTS_F_COUNTDOWN_TO_START_HPP__


#include "fragment.hpp"


struct FCountdownToStartData : public FragmentData {};

class FCountdownToStart : public Fragment {
  public:
    FCountdownToStart(Fragment& parent, FragmentData& parentData);

    virtual void rebuild(const FragmentSpec& spec) override;
    virtual void cleanUp() override;

  private:
    void onQuit();

    FCountdownToStartData m_data;
    int m_stateId;
};


#endif
