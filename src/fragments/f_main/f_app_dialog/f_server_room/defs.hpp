#ifndef __PROCALC_FRAGMENTS_F_SERVER_ROOM_DEFS_HPP__
#define __PROCALC_FRAGMENTS_F_SERVER_ROOM_DEFS_HPP__


#include <QString>
#include "utils.hpp"


namespace youve_got_mail {


const QString exitDoorCode = QString::number(1000 + (randomSeed() % 9000));


}


#endif
