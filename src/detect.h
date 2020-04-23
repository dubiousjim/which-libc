#ifndef DETECT_H
#define DETECT_H 1

/* References
 * https://web.archive.org/web/nadeausoftware.com/articles/2012/01/c_c_tip_how_use_compiler_predefined_macros_detect_operating_system
 * https://sourceforge.net/p/predef/wiki/Home/
 * https://www.boost.org/doc/libs/release/libs/predef/
 * https://www.msys2.org/wiki/Porting/
 */

#include "macro.h"

/* Detect OS/Platform */

#if defined(__ANDROID__)
# ifndef __unix__
#  error "__ANDROID__ without __unix__"
# endif
# ifndef __linux__
#  error "__ANDROID__ without __linux__"
# endif
# include <android/api-level.h>
# undef __android__
# define __android__ __ANDROID_API__ /* 1..11 */

#elif defined(__APPLE__) && defined(__MACH__)
# ifdef __unix__
#  error "__APPLE__ with __unix__"
# endif
/* gcc and clang also define __APPLE_CC__ */
/* See https://sourceforge.net/p/predef/mailman/message/34497133/ */
# include "TargetConditionals.h"
# if defined(TARGET_IPHONE_SIMULATOR) && (TARGET_IPHONE_SIMULATOR != 0)
    /* APPLE_SIMULATOR */
# elif defined(TARGET_OS_IPHONE) && (TARGET_OS_IPHONE != 0)
    /* APPLE_IPHONE */
# elif defined(TARGET_OS_WATCH) && (TARGET_OS_WATCH != 0)
    /* APPLE_WATCH */
# elif defined(TARGET_OS_MAC) && (TARGET_OS_MAC != 0)
#  define __unix__ 1
#  include <sys/param.h>
#  ifndef BSD
#   error "__APPLE__ without BSD"
#  endif
#  if defined(__ENVIRONMENT_MAC_OS_X_VERSION_MIN_REQUIRED__)
#   undef __osx__
#   define __osx__ (__ENVIRONMENT_MAC_OS_X_VERSION_MIN_REQUIRED__/100) /* will be 1010 for 10.10.5 */
    /*
     * there's also a __MAC_OS_X_VERSION_MAX_ALLOWED
     * OS X 10.5 Leopard is Darwin 9.x
     * OS X 10.6 Snow Leopard is Darwin 10.x ...
     * OS X 10.7 Lion
     * OS X 10.8 Mountain Lion
     * OS X 10.9 Mavericks
     * OS X 10.10 Yosemite (Oct 2014)
     * OS X 10.11 El Capitan (Sept 2015)
     * OS X 10.12 Sierra (Sept 2016)
     * OS X 10.13 High Sierra (Sept 2017)
     * OS X 10.14 Mojave (Sept 2018)
     * OS X 10.15 Catalina (Oct 2019, 64-bit only)
     * ...
     */
#  endif
# else
#  error "Unknown Apple platform"
# endif
# if !defined(__osx__)
#  error "Unsupported Apple platform"
# endif

#elif defined(__FreeBSD__)
# ifndef __unix__
#  error "__FreeBSD__ without __unix__"
# endif
# include <sys/param.h>
/* include <osreldate.h> */
# ifndef BSD
#  error "__FreeBSD__ without BSD"
# endif
# ifndef __FreeBSD_version
#  error "__FreeBSD_version undefined"
# endif
# undef __FreeBSD__
# if (__FreeBSD_version < 491000)
#  define __FreeBSD__ ((__FreeBSD_version/100000)*90 + (__FreeBSD_version/10000))
# elif (__FreeBSD_version >= 491000) && (__FreeBSD_version < 492000)
#  define __FreeBSD__ 410 /* May 2004 */
# elif (__FreeBSD_version >= 492000) && (__FreeBSD_version < 493000)
#  define __FreeBSD__ 411 /* Dec 2004 */
# else
#  define __FreeBSD__ (__FreeBSD_version/10000) /* will be 1101L for 11.01 */
# endif

#elif defined(__NETBSD__)
# ifndef __unix__
#  error "__NETBSD__ without __unix__"
# endif
# include <sys/param.h>
# ifndef BSD
#  error "__NETBSD__ without BSD"
# endif
# ifndef __NETBSD_version
#  error "__NETBSD_version undefined"
# endif
# undef __NetBSD__
# if __NETBSD_version < 500000
#  define __NetBSD__ ((__NETBSD_version/100000)*90 + (__NETBSD_version/10000))
# else
#  define __NetBSD__ (__NETBSD_version/1000)
# endif

#elif defined(__NetBSD__)
# ifndef __unix__
#  error "__NetBSD__ without __unix__"
# endif
# include <sys/param.h>
# ifndef BSD
#  error "__NetBSD__ without BSD"
# endif
# ifndef __NetBSD_Version
#  error "__NetBSD_Version undefined"
# endif
# undef __NetBSD__
# define __NetBSD__ (__NetBSD_Version/1000000L) /* will be VVRR for VV.RR */

#elif defined(__OpenBSD__)
# ifndef __unix__
#  error "__OpenBSD__ without __unix__"
# endif
# include <sys/param.h>
# ifndef BSD
#  error "__OpenBSD__ without BSD"
# endif
# undef __OpenBSD__
# if defined(OpenBSD2_0) /* Oct 1996 */
#  define __OpenBSD__ 200
# elif defined(OpenBSD2_1) /* June 1997 */
#  define __OpenBSD__ 201
# elif defined(OpenBSD2_2) /* Dec 1997 */
#  define __OpenBSD__ 202
# elif defined(OpenBSD2_3) /* May 1998 */
#  define __OpenBSD__ 203
# elif defined(OpenBSD2_4) /* Dec 1998 */
#  define __OpenBSD__ 204
# elif defined(OpenBSD2_5) /* May 1999 */
#  define __OpenBSD__ 205
# elif defined(OpenBSD2_6) /* Dec 1999 */
#  define __OpenBSD__ 206
# elif defined(OpenBSD2_7) /* June 2000 */
#  define __OpenBSD__ 207
# elif defined(OpenBSD2_8) /* Dec 2000 */
#  define __OpenBSD__ 208
# elif defined(OpenBSD2_9) /* June 2001 */
#  define __OpenBSD__ 209
# elif defined(OpenBSD3_0) /* Dec 2001 */
#  define __OpenBSD__ 300
# elif defined(OpenBSD3_1) /* May 2002 */
#  define __OpenBSD__ 301
# elif defined(OpenBSD3_2) /* Nov 2002 */
#  define __OpenBSD__ 302
# elif defined(OpenBSD3_3) /* May 2003 */
#  define __OpenBSD__ 303
# elif defined(OpenBSD3_4) /* Nov 2003 */
#  define __OpenBSD__ 304
# elif defined(OpenBSD3_5) /* May 2004 */
#  define __OpenBSD__ 305
# elif defined(OpenBSD3_6) /* Nov 2004 */
#  define __OpenBSD__ 306
# elif defined(OpenBSD3_7) /* May 2005 */
#  define __OpenBSD__ 307
# elif defined(OpenBSD3_8) /* Nov 2005 */
#  define __OpenBSD__ 308
# elif defined(OpenBSD3_9) /* May 2006 */
#  define __OpenBSD__ 309
# elif defined(OpenBSD4_0) /* Nov 2006 */
#  define __OpenBSD__ 400
# elif defined(OpenBSD4_1) /* May 2007 */
#  define __OpenBSD__ 401
# elif defined(OpenBSD4_2) /* Nov 2007 */
#  define __OpenBSD__ 402
# elif defined(OpenBSD4_3) /* May 2008 */
#  define __OpenBSD__ 403
# elif defined(OpenBSD4_4) /* Nov 2008 */
#  define __OpenBSD__ 404
# elif defined(OpenBSD4_5) /* May 2009 */
#  define __OpenBSD__ 405
# elif defined(OpenBSD4_6) /* Oct 2009 */
#  define __OpenBSD__ 406
# elif defined(OpenBSD4_7) /* May 2010 */
#  define __OpenBSD__ 407
# elif defined(OpenBSD4_8) /* Nov 2010 */
#  define __OpenBSD__ 408
# elif defined(OpenBSD4_9) /* May 2011 */
#  define __OpenBSD__ 409
# elif defined(OpenBSD5_0) /* Nov 2011 */
#  define __OpenBSD__ 500
# elif defined(OpenBSD5_1) /* May 2012 */
#  define __OpenBSD__ 501
# elif defined(OpenBSD5_2) /* Nov 2012 */
#  define __OpenBSD__ 502
# elif defined(OpenBSD5_3) /* May 2013 */
#  define __OpenBSD__ 503
# elif defined(OpenBSD5_4) /* Nov 2013 */
#  define __OpenBSD__ 504
# elif defined(OpenBSD5_5) /* May 2014 */
#  define __OpenBSD__ 505
# elif defined(OpenBSD5_6) /* Nov 2014 */
#  define __OpenBSD__ 506
# elif defined(OpenBSD5_7) /* May 2015 */
#  define __OpenBSD__ 507
# elif defined(OpenBSD5_8) /* Oct 2015 */
#  define __OpenBSD__ 508
# elif defined(OpenBSD5_9) /* Mar 2016 */
#  define __OpenBSD__ 509
# elif defined(OpenBSD6_0) /* Sept 2016 */
#  define __OpenBSD__ 600
# elif defined(OpenBSD6_1) /* Apr 2017 */
#  define __OpenBSD__ 601
# elif defined(OpenBSD6_2) /* Oct 2017 */
#  define __OpenBSD__ 602
# elif defined(OpenBSD6_3) /* Apr 2018 */
#  define __OpenBSD__ 603
# elif defined(OpenBSD6_4) /* Oct 2018 */
#  define __OpenBSD__ 604
# elif defined(OpenBSD6_5) /* Apr 2019 */
#  define __OpenBSD__ 605
# elif defined(OpenBSD6_6) /* Oct 2019 */
#  define __OpenBSD__ 606
# elif defined(OpenBSD6_7) /* unreleased */
#  define __OpenBSD__ 607
# elif defined(OpenBSD6_8) /* unreleased */
#  define __OpenBSD__ 608
# elif defined(OpenBSD6_9) /* unreleased */
#  define __OpenBSD__ 609
# else
#  error "Unknown OpenBSD version"
# endif

#elif defined(__DragonFly__)
# ifndef __unix__
#  error "__DragonFly__ without __unix__"
# endif
# include <sys/param.h>
# ifndef BSD
#  error "__DragonFly__ without BSD"
# endif
 /* __DragonFly__ just identifies; BSD is of form YYYYMM */

#elif (defined(__unix__) || defined(__unix)) && !defined(_WIN32) /* Cygwin's GCC defines __CYGWIN__ and __unix__ even when building Windows apps */
# if !defined(__unix__)
#  define __unix__ 1
# endif

# include <sys/param.h>
# if defined(BSD)
#  ifdef __linux__
#   error "BSD with __linux__"
#  endif
#  error "Unknown BSD platform"

# elif defined(__MSYS__)
#  ifdef __linux__
#   error "__MSYS__ with __linux__"
#  endif
#  ifdef __GLIBC__
#   error "__MSYS__ with __GLIBC__"
#  endif
#  ifdef __mingw__
#   error "__MSYS__ with __MINGW64/32__"
#  endif
#  ifdef __msvc__
#   error "__MSYS__ with _MSC_VER"
#  endif
#  ifndef __CYGWIN__
#   error "__MSYS__ without __CYGWIN__"
#  endif
   /* TODO determine version? */
#  define __msys__ 1

# elif defined(__CYGWIN__) /* also defined in MSYS2 */
  /* Cygwin in Posix mode, uses newlib, only generates 32-bit binaries */
#  ifdef __linux__
#   error "__CYGWIN__ with __linux__"
#  endif
#  ifdef __GLIBC__
#   error "__CYGWIN__ with __GLIBC__"
#  endif
#  ifdef __mingw__
#   error "__GYGWIN__ with __MINGW64/32__"
#  endif
#  ifdef __msvc__
#   error "__CYGWIN__ with _MSC_VER"
#  endif
#  include <cygwin/version.h>
#  define __cygwin__ (CYGWIN_VERSION_API_MAJOR*100 + CYGWIN_VERSION_API_MINOR)

# elif defined(__linux__) || defined(__linux) || defined(__gnu_linux__) || defined(linux)
#  if !defined(__linux__)
#   define __linux__ 1
#  endif

# else
 /*
  * __GNU__: identifies GNU/Hurd, so does __gnu_hurd__
  * __FreeBSD_kernel__ && __GLIBC__: identifies GNU/kFreeBSD
  */
#  error "Unknown Unix platform"
# endif

#elif defined(_WIN32)
  /* either __msvc__
   * or Cygwin's __gcc__ targetting Windows (-mwin32)
   * or MinGW's __gcc__ targetting Windows (-mwin32), with -m32/-m64
   * or __clang__ targetting Windows (host triple ...-win32) or MinGW (...-mingw32), with -m32/-m64
   */
# if defined(_WIN64)
#  define __windows__ 64
# else
#  define __windows__ 32
# endif
  /* Gygwin's GCC defines __CYGWIN__ and __unix__ even when building Windows apps */
# undef __unix__

#elif defined(_WIN64)
# error "_WIN64 without _WIN32"

#elif defined(__MSYS__)
# error "__MSYS__ without __unix__"

#elif defined(__CYGWIN__)
# error "__CYGWIN__ without __unix__"

#else
# error "Unknown platform"
#endif



/* Detect _POSIX_VERSION, _XOPEN_VERSION, __LSB_VERSION__ */

#if defined(__unix__)
# include <unistd.h>
#endif


/* Detect Libc */

#if __has_include(<features.h>)
# include <features.h> /* for GNU and uClibc */
#elif __has_include_broken__
# include <limits.h> /* try to safely get Gnu's <features.h> if our compiler lacks __has_include */
#endif

#if __has_include(<sys/cdefs.h>)
# include <sys/cdefs.h> /* for Bionic */
#elif __has_include_broken__ && defined(__ANDROID__)
# include <sys/types.h> /* Posix header, try to safely get Gnu's <sys/cdefs.h> if our compiler lacks __has_include */
#endif

#if defined(__windows__)
 /* skip */

#elif defined(__BIONIC__)
 /* just identifies */
# ifndef __ANDROID__
#  error "__BIONIC__ without Android"
# endif

#elif defined(__ANDROID__)
# error "Android without __BIONIC__"

#elif defined(BSD) /* includes Apple */
# if defined(__KLIBC__)
#  error "BSD with __KLIBC__"
# elif defined(__UCLIBC__)
#  error "BSD with __UCLIBC__"
# elif defined(__GLIBC__)
#  error "BSD with __GLIBC__"
# elif defined(__GNU_LIBRARY__)
#  error "BSD with __GNU_LIBRARY__"
# endif

#elif defined(__KLIBC__)
# undef __KLIBC__
# define __KLIBC__ ((__KLIBC_VERSION__ >> 24)*100 + __KLIBC_MINOR__) /* will be VVRR */

#elif defined(__UCLIBC__)
# ifndef __GLIBC__
#  error "__UCLIBC__ without __GLIBC__"
# endif
# undef __UCLIBC__
# define __UCLIBC__ (__UCLIBC_MAJOR__ * 100 + __UCLIBC_MINOR__)

#elif defined(__GLIBC__) && defined(__GLIBC_MINOR__)
/* release history at https://sourceware.org/glibc/wiki/Glibc%20Timeline and https://www.gnu.org/software/libc/ */
# if __GNU_LIBRARY__ < 6
#  error "Unknown glibc version"
# endif
# define __glibc__ (__GLIBC__ * 100 + __GLIBC_MINOR__)
# if __glibc__ < 200
#  error "glibc must be at least 2.0 (Jan 1997)"
# endif
#elif defined(__GNU_LIBRARY__) || defined(__GLIBC__)
# error "Unknown glibc version"

#else /* maybe __musl__ */
# ifndef __unix__
#  error "Unknown platform"
# endif
# ifndef __linux__
#  error "Unknown Unix platform"
# endif
# if __has_include(<features.h> && __has_include(<unistd.h>)
#  include <unistd.h>
#  if !defined(__GLIBC__) && defined(_CS_GNU_LIBC_VERSION)
   /* FIXME, other libcs may define this too */
#   define __musl__ 1
#  endif
# endif
# if !defined(__musl__)
#  error "Unknown Linux libc"
# endif
#endif /* maybe __musl__ */

#endif /* DETECT_H */
