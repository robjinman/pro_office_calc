#include "platform.hpp"
#ifdef WIN32
#include <Windows.h>
#else
#include <unistd.h>
#endif

#ifdef WIN32
void sleepThread(long millis) {
  Sleep(millis);
}
#else
void sleepThread(long millis) {
  usleep(millis * 1000);
}
#endif
