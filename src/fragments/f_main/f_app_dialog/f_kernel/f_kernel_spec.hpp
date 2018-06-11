#ifndef __PROCALC_FRAGMENTS_F_KERNEL_SPEC_HPP__
#define __PROCALC_FRAGMENTS_F_KERNEL_SPEC_HPP__


#include "fragment_spec.hpp"


struct FKernelSpec : public FragmentSpec {
  FKernelSpec()
    : FragmentSpec("FKernel", {}) {}
};


#endif
