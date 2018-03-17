#ifndef __PROCALC_FRAGMENTS_F_DESKTOP_SPEC_HPP__
#define __PROCALC_FRAGMENTS_F_DESKTOP_SPEC_HPP__


#include <string>
#include <vector>
#include "fragment_spec.hpp"
#include "fragments/f_main/f_desktop/f_server_room_init/f_server_room_init_spec.hpp"


struct FDesktopSpec : public FragmentSpec {
  struct Icon {
    std::string image;
    std::string text;
    std::string eventName;
  };

  FDesktopSpec()
    : FragmentSpec("FDesktop", {
      &serverRoomInitSpec
    }) {}

  FServerRoomInitSpec serverRoomInitSpec;
  std::vector<Icon> icons;
};


#endif
