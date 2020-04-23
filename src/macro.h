#ifndef MACRO_H
#define MACRO_H 1

/* Detect Compiler (currently: gcc, clang, MinGW[-w64], MS Visual C
 * References
 * https://sourceforge.net/p/predef/wiki/Compilers
 */
/************************************************************************/
#if defined(__clang__)
  /* __GNUC__ will also be defined */
  /* release history at http://releases.llvm.org/ */
# undef __clang__
# define __clang__ (__clang_major__ * 100 + __clang_minor__)
# if __clang__ < 305
#  error "clang must be at least 3.5 (Sept 2014)"
  /* first to support _Pragma("GCC error \"msg\""), and __has_attribute be target-sensitive
   * also __alignof fixed for incomplete arrays
   * 3.4 (Jan 2014) full range of multiprecision math, also checked math (but generics not until 3.8)
   * 3.3 (Jun 2013) _Alignas follows C11 rules
   */
# endif

#elif defined(__INTEL_COMPILER)
  /* __GNUC__ will also be defined */
# error "Unknown Intel compiler"
#elif defined(__ICL) || defined(__ICC) || defined(__ECC)
# error "Unknown Intel compiler"

#elif defined(__GNUC__)
  /* __GNUC__ is already the major version */
  /* release history at https://gcc.gnu.org/ and https://gcc.gnu.org/news.html */
# define __gcc__ (__GNUC__ * 100 + __GNUC_MINOR__)
# if __gcc__ < 500
#  error "GCC must be at least 5.0 (Dec 2015)"
  /* first with __has_attribute and __has_include, __builtin_MATH_overflow
   * 4.9 (Apr 2014) auto_type
   * 4.8 (Mar 2013)_Pragma("GCC error \"msg\"")
   * 4.7 (Mar 2012) builtin_const_p understood by inliner, builtin_assume_aligned, _Noreturn|_Alignas|_Alignof
   * 4.6 (Mar 2011) _Pragma("GCC diagnostic error|ignored|push|pop")
   * 4.5 (Apr 2010) C99 mostly finished, deprecated(msg) attrib
   */
# endif

#elif defined(_MSC_VER)
  /* Microsoft Visual C, also defined by clang
   * References
   * https://en.wikipedia.org/wiki/Microsoft_Visual_C%2B%2B
   * https://stackoverflow.com/questions/70013/how-to-detect-if-im-compiling-code-with-a-particular-visual-studio-version
   * https://docs.microsoft.com/en-us/cpp/preprocessor/predefined-macros?view=vs-2019
   */
# define __msvc__ (_MSC_VER)

#elif defined(__MINGW64__)
# include <windef.h> /* or windows.h or _mingw.h */
# define __mingw64__ (__MINGW64_VERSION_MAJOR * 100 + __MINGW64_VERSION_MINOR)
# define __mingw__ 64 /* targeting */
#elif defined(__MINGW32__) /* includes MinGW-w64 building 32bit */
# include <windef.h> /* or windows.h or _mingw.h */
# if defined(__MINGW64_VERSION_MAJOR)
#  if (__MINGW64_VERSION_MAJOR != __MINGW32_VERSION_MAJOR) || (__MINGW64_VERSION_MINOR != __MINGW32_VERSION_MINOR)
#   error "__MINGW64_VERSION differs from __MINGW32_VERSION"
#  endif
#  define __mingw64__ (__MINGW64_VERSION_MAJOR * 100 + __MINGW64_VERSION_MINOR)
# else
#  define __mingw32__ (__MINGW32_VERSION_MAJOR * 100 + __MINGW32_VERSION_MINOR)
# endif
# define __mingw__ 32 /* targeting */

#else
# error "Unknown compiler"
/*
 * Amsterdam Compiler Kit/ACK (Minix < 3.2)
 * Borland C Builder or Turbo C
 * Wind River Diab/DCC
 * Digital Mars/DMC (was Zorland)
 * Oracle Solaris Studio/SUNPRO
 * Stratus VOS
 * Small Device C Compiler/SDCC
 * Tiny CC
 * Watcom C
 * ...and many others
 */
#endif /* detect compiler */

/* GCC/clang's feature checking macros */
/***************************************/
#if defined(lint) || !defined(__has_attribute)
  /* gcc 5.0 and clang (2.9, target-aware since 3.5) have this */
# define __has_attribute(f) 0
#endif
#ifndef __has_include
  /* gcc 5.0 and clang 2.7 have this */
# define __has_include_broken__ 1
# define __has_include(f) 0
#endif
#ifndef __has_extension
/* absent on gcc, clang 3.0 has */
# define __has_extension __has_feature
#endif
#ifndef __has_feature
/* absent on gcc, clang 2.6 has */
# define __has_feature(f) 0
#endif
#ifndef __has_builtin
/* absent on gcc, clang 2.6 has */
# define __has_builtin(func) 0
#endif
#ifndef __has_warning
/* absent on gcc, clang 3.0 has */
# define __has_warning(str) 0
#endif

#if defined(lint) || !((defined(__gcc__) && __gcc__ > 299) || (defined(__clang__) && __clang__ >= 300)) /* clang matches gcc with 3.0 */
# define __extension__
#endif

/* Macro Utilities */
/*******************/
#if defined(__STDC_VERSION__) && (__STDC_VERSION__ >= 201112L)
# define MACRO_ASSERT(exp, msg) _Static_assert((exp), (msg));
#elif defined(lint)
# define MACRO_ASSERT(exp, msg) /* FIXME */
#elif (defined(__gcc__) && __gcc__ >= 406) || __has_extension(c_static_assert) /* clang 3.0 */ || defined(_Static_assert)
# define MACRO_ASSERT(exp, msg) __extension__ _Static_assert((exp), (msg));
#elif defined(__COUNTER__) /* gcc 4.3, clang 2.6 */
  /* from FreeBSD sys/cdefs.h */
# define __MACRO_AUX1_ASSERT(exp,i) __MACRO_AUX2_ASSERT(exp,i)
# define __MACRO_AUX2_ASSERT(exp,i) typedef char __assert_ ## i[(exp) ? 1 : -1] MAYBE_UNUSED
# define MACRO_ASSERT(exp, msg) __MACRO_AUX1_ASSERT((exp), __COUNTER__);
#else
  /* from glibc sys/cdefs.h */
# define MACRO_ASSERT(exp, msg) \
   extern int (*__Static_assert_function (void))[!!sizeof (struct { int __error_if_negative: (exp) ? 2 : -1; })];
  /* see also http://jonjagger.blogspot.co.uk/2017/07/compile-time-assertions-in-c.html
   * and https://stackoverflow.com/questions/3385515/static-assert-in-c#3385694 */
#endif

/* check clang: since 2.6, but reports false for __has_builtin(__builtin_choose_expr) */
#if !defined(lint) && ((defined(__gcc__) && __gcc__ >= 301) || (defined(__clang__) && __clang__ >= 206))
  /* advantage is that this can return an lvalue, and results aren't promoted to a common type */
# define MACRO_CHOOSE(exp, yes, no) __builtin_choose_expr((exp), (yes), (no))
#else
# define MACRO_CHOOSE(exp, yes, no) ((exp) ? (yes) : (no))
#endif

#define __MACRO_AUX_CONCAT(x,y) x ## y
#define MACRO_CONCAT(x,y) __MACRO_AUX_CONCAT(x,y) /* expand then concat pieces of a symbol */

#define __MACRO_AUX_STRING(x) #x
#define MACRO_STRING(x) __MACRO_AUX_STRING((x)) /* expand then stringify */

#define __MACRO_AUX_PRAGMA(pragma) _Pragma(#pragma)
#define MACRO_WARN(msg) __MACRO_AUX_PRAGMA(GCC warning msg)
#define MACRO_ERROR(msg) __MACRO_AUX_PRAGMA(GCC error msg)
#define MACRO_INFO(msg) __MACRO_AUX_PRAGMA(message (/* "prefix:" */ msg)) /* clang makes these warnings */

#if !defined(lint) && ((defined(__gcc__) && __gcc__ >= 405) || __has_extension(attribute_deprecated_with_message)) /* clang 2.9; intelc? */
# define DEPRECATED(msg) __attribute__((deprecated(msg)))
#elif __has_attribute(deprecated) /* gcc 3.1 */
# define DEPRECATED(msg) __attribute__((deprecated))
#else
# define DEPRECATED(msg)
#endif

#endif /* MACRO_H */
