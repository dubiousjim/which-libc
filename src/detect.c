#include "detect.h"
#include <stdint.h>

#if defined(__windows__)
/* includes __msvc__, __mingw64/32__ (__mingw__ = which of 32/64 is targeted) */
const char* target_os = "Windows";
uint64_t target_version = 0;
#elif defined(__msys__)
const char* target_os = "MSYS";
uint64_t target_version = 0;
#elif defined(__cygwin__)
const char* target_os = "Cygwin";
uint64_t target_version = __cygwin__;
#elif defined(__FreeBSD__)
const char* target_os = "FreeBSD";
uint64_t target_version = __FreeBSD__;
#elif defined(__NetBSD__)
const char* target_os = "NetBSD";
uint64_t target_version = __NetBSD__;
#elif defined(__OpenBSD__)
const char* target_os = "OpenBSD";
uint64_t target_version = __OpenBSD__;
#elif defined(__DragonFly__)
const char* target_os = "DragonFly";
uint64_t target_version = 0;
#elif defined(__osx__)
const char* target_os = "OSX";
uint64_t target_version = __osx__;
#elif defined(__android__)
const char* target_os = "Android";
uint64_t target_version = __android__;
#elif defined(__linux__)
# if defined(__glibc__)
const char* target_os = "Gnu/Linux";
uint64_t target_version = __glibc__;
# elif defined(__musl__)
const char* target_os = "Musl/Linux";
uint64_t target_version = 0;
# else
/* includes __KLIBC__, __UCLIBC__ */
const char* target_os = "Linux";
uint64_t target_version = 0;
# endif
#else
const char* target_os = "";
uint64_t target_version = 0;
#endif
