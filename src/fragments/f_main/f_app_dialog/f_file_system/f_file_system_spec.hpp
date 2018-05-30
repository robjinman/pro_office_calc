#ifndef __PROCALC_FRAGMENTS_F_FILE_SYSTEM_SPEC_HPP__
#define __PROCALC_FRAGMENTS_F_FILE_SYSTEM_SPEC_HPP__


#include "fragment_spec.hpp"


struct FFileSystemSpec : public FragmentSpec {
  FFileSystemSpec()
    : FragmentSpec("FFileSystem", {}) {}
};


#endif
