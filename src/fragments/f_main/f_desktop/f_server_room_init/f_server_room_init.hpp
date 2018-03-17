#ifndef __PROCALC_FRAGMENTS_F_SERVER_ROOM_INIT_HPP__
#define __PROCALC_FRAGMENTS_F_SERVER_ROOM_INIT_HPP__


#include "fragment.hpp"
#include "qt_obj_ptr.hpp"


struct FServerRoomInitData : public FragmentData {};

class FServerRoomInit : public Fragment {
  public:
    FServerRoomInit(Fragment& parent, FragmentData& parentData, const CommonFragData& commonData);

    virtual void reload(const FragmentSpec& spec) override;
    virtual void cleanUp() override;

    virtual ~FServerRoomInit() override;

  private:
    FServerRoomInitData m_data;

    int m_launchEventId = -1;
};


#endif
