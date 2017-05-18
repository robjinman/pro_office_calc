#ifndef __PROCALC_FRAGMENT_FACTORY_HPP__
#define __PROCALC_FRAGMENT_FACTORY_HPP__


#include <string>


class Fragment;
class FragmentData;


Fragment* constructFragment(const std::string& name, Fragment& parent, FragmentData& parentData);


#endif
