
// This is a generated file. Do not edit!

#ifndef UTIL_CONFIG_COMPILER_DETECTION_H
#define UTIL_CONFIG_COMPILER_DETECTION_H

#ifndef __cplusplus
#  define UTIL_CONFIG_COMPILER_IS_Intel 0
#  define UTIL_CONFIG_COMPILER_IS_PathScale 0
#  define UTIL_CONFIG_COMPILER_IS_Embarcadero 0
#  define UTIL_CONFIG_COMPILER_IS_Borland 0
#  define UTIL_CONFIG_COMPILER_IS_Watcom 0
#  define UTIL_CONFIG_COMPILER_IS_OpenWatcom 0
#  define UTIL_CONFIG_COMPILER_IS_SunPro 0
#  define UTIL_CONFIG_COMPILER_IS_HP 0
#  define UTIL_CONFIG_COMPILER_IS_Compaq 0
#  define UTIL_CONFIG_COMPILER_IS_zOS 0
#  define UTIL_CONFIG_COMPILER_IS_XLClang 0
#  define UTIL_CONFIG_COMPILER_IS_XL 0
#  define UTIL_CONFIG_COMPILER_IS_VisualAge 0
#  define UTIL_CONFIG_COMPILER_IS_PGI 0
#  define UTIL_CONFIG_COMPILER_IS_Cray 0
#  define UTIL_CONFIG_COMPILER_IS_TI 0
#  define UTIL_CONFIG_COMPILER_IS_Fujitsu 0
#  define UTIL_CONFIG_COMPILER_IS_GHS 0
#  define UTIL_CONFIG_COMPILER_IS_TinyCC 0
#  define UTIL_CONFIG_COMPILER_IS_Bruce 0
#  define UTIL_CONFIG_COMPILER_IS_SCO 0
#  define UTIL_CONFIG_COMPILER_IS_ARMCC 0
#  define UTIL_CONFIG_COMPILER_IS_AppleClang 0
#  define UTIL_CONFIG_COMPILER_IS_ARMClang 0
#  define UTIL_CONFIG_COMPILER_IS_Clang 0
#  define UTIL_CONFIG_COMPILER_IS_GNU 0
#  define UTIL_CONFIG_COMPILER_IS_MSVC 0
#  define UTIL_CONFIG_COMPILER_IS_ADSP 0
#  define UTIL_CONFIG_COMPILER_IS_IAR 0
#  define UTIL_CONFIG_COMPILER_IS_SDCC 0
#  define UTIL_CONFIG_COMPILER_IS_MIPSpro 0

#  if defined(__INTEL_COMPILER) || defined(__ICC)
#    undef UTIL_CONFIG_COMPILER_IS_Intel
#    define UTIL_CONFIG_COMPILER_IS_Intel 1

#  elif defined(__PATHCC__)
#    undef UTIL_CONFIG_COMPILER_IS_PathScale
#    define UTIL_CONFIG_COMPILER_IS_PathScale 1

#  elif defined(__BORLANDC__) && defined(__CODEGEARC_VERSION__)
#    undef UTIL_CONFIG_COMPILER_IS_Embarcadero
#    define UTIL_CONFIG_COMPILER_IS_Embarcadero 1

#  elif defined(__BORLANDC__)
#    undef UTIL_CONFIG_COMPILER_IS_Borland
#    define UTIL_CONFIG_COMPILER_IS_Borland 1

#  elif defined(__WATCOMC__) && __WATCOMC__ < 1200
#    undef UTIL_CONFIG_COMPILER_IS_Watcom
#    define UTIL_CONFIG_COMPILER_IS_Watcom 1

#  elif defined(__WATCOMC__)
#    undef UTIL_CONFIG_COMPILER_IS_OpenWatcom
#    define UTIL_CONFIG_COMPILER_IS_OpenWatcom 1

#  elif defined(__SUNPRO_C)
#    undef UTIL_CONFIG_COMPILER_IS_SunPro
#    define UTIL_CONFIG_COMPILER_IS_SunPro 1

#  elif defined(__HP_cc)
#    undef UTIL_CONFIG_COMPILER_IS_HP
#    define UTIL_CONFIG_COMPILER_IS_HP 1

#  elif defined(__DECC)
#    undef UTIL_CONFIG_COMPILER_IS_Compaq
#    define UTIL_CONFIG_COMPILER_IS_Compaq 1

#  elif defined(__IBMC__) && defined(__COMPILER_VER__)
#    undef UTIL_CONFIG_COMPILER_IS_zOS
#    define UTIL_CONFIG_COMPILER_IS_zOS 1

#  elif defined(__ibmxl__) && defined(__clang__)
#    undef UTIL_CONFIG_COMPILER_IS_XLClang
#    define UTIL_CONFIG_COMPILER_IS_XLClang 1

#  elif defined(__IBMC__) && !defined(__COMPILER_VER__) && __IBMC__ >= 800
#    undef UTIL_CONFIG_COMPILER_IS_XL
#    define UTIL_CONFIG_COMPILER_IS_XL 1

#  elif defined(__IBMC__) && !defined(__COMPILER_VER__) && __IBMC__ < 800
#    undef UTIL_CONFIG_COMPILER_IS_VisualAge
#    define UTIL_CONFIG_COMPILER_IS_VisualAge 1

#  elif defined(__PGI)
#    undef UTIL_CONFIG_COMPILER_IS_PGI
#    define UTIL_CONFIG_COMPILER_IS_PGI 1

#  elif defined(_CRAYC)
#    undef UTIL_CONFIG_COMPILER_IS_Cray
#    define UTIL_CONFIG_COMPILER_IS_Cray 1

#  elif defined(__TI_COMPILER_VERSION__)
#    undef UTIL_CONFIG_COMPILER_IS_TI
#    define UTIL_CONFIG_COMPILER_IS_TI 1

#  elif defined(__FUJITSU) || defined(__FCC_VERSION) || defined(__fcc_version)
#    undef UTIL_CONFIG_COMPILER_IS_Fujitsu
#    define UTIL_CONFIG_COMPILER_IS_Fujitsu 1

#  elif defined(__ghs__)
#    undef UTIL_CONFIG_COMPILER_IS_GHS
#    define UTIL_CONFIG_COMPILER_IS_GHS 1

#  elif defined(__TINYC__)
#    undef UTIL_CONFIG_COMPILER_IS_TinyCC
#    define UTIL_CONFIG_COMPILER_IS_TinyCC 1

#  elif defined(__BCC__)
#    undef UTIL_CONFIG_COMPILER_IS_Bruce
#    define UTIL_CONFIG_COMPILER_IS_Bruce 1

#  elif defined(__SCO_VERSION__)
#    undef UTIL_CONFIG_COMPILER_IS_SCO
#    define UTIL_CONFIG_COMPILER_IS_SCO 1

#  elif defined(__ARMCC_VERSION) && !defined(__clang__)
#    undef UTIL_CONFIG_COMPILER_IS_ARMCC
#    define UTIL_CONFIG_COMPILER_IS_ARMCC 1

#  elif defined(__clang__) && defined(__apple_build_version__)
#    undef UTIL_CONFIG_COMPILER_IS_AppleClang
#    define UTIL_CONFIG_COMPILER_IS_AppleClang 1

#  elif defined(__clang__) && defined(__ARMCOMPILER_VERSION)
#    undef UTIL_CONFIG_COMPILER_IS_ARMClang
#    define UTIL_CONFIG_COMPILER_IS_ARMClang 1

#  elif defined(__clang__)
#    undef UTIL_CONFIG_COMPILER_IS_Clang
#    define UTIL_CONFIG_COMPILER_IS_Clang 1

#  elif defined(__GNUC__)
#    undef UTIL_CONFIG_COMPILER_IS_GNU
#    define UTIL_CONFIG_COMPILER_IS_GNU 1

#  elif defined(_MSC_VER)
#    undef UTIL_CONFIG_COMPILER_IS_MSVC
#    define UTIL_CONFIG_COMPILER_IS_MSVC 1

#  elif defined(__VISUALDSPVERSION__) || defined(__ADSPBLACKFIN__) || defined(__ADSPTS__) || defined(__ADSP21000__)
#    undef UTIL_CONFIG_COMPILER_IS_ADSP
#    define UTIL_CONFIG_COMPILER_IS_ADSP 1

#  elif defined(__IAR_SYSTEMS_ICC__) || defined(__IAR_SYSTEMS_ICC)
#    undef UTIL_CONFIG_COMPILER_IS_IAR
#    define UTIL_CONFIG_COMPILER_IS_IAR 1

#  elif defined(__SDCC_VERSION_MAJOR) || defined(SDCC)
#    undef UTIL_CONFIG_COMPILER_IS_SDCC
#    define UTIL_CONFIG_COMPILER_IS_SDCC 1

#  endif

#  if UTIL_CONFIG_COMPILER_IS_GNU

#    if !((__GNUC__ * 100 + __GNUC_MINOR__) >= 304)
#      error Unsupported compiler version
#    endif

#    define UTIL_CONFIG_COMPILER_VERSION_MAJOR (__GNUC__)
#    if defined(__GNUC_MINOR__)
#      define UTIL_CONFIG_COMPILER_VERSION_MINOR (__GNUC_MINOR__)
#    endif
#    if defined(__GNUC_PATCHLEVEL__)
#      define UTIL_CONFIG_COMPILER_VERSION_PATCH (__GNUC_PATCHLEVEL__)
#    endif

#    if (__GNUC__ * 100 + __GNUC_MINOR__) >= 304 && defined(__STDC_VERSION__) && __STDC_VERSION__ >= 199901L
#      define UTIL_CONFIG_COMPILER_C_RESTRICT 1
#    else
#      define UTIL_CONFIG_COMPILER_C_RESTRICT 0
#    endif

#    if (__GNUC__ * 100 + __GNUC_MINOR__) >= 406 && defined(__STDC_VERSION__) && __STDC_VERSION__ >= 201000L
#      define UTIL_CONFIG_COMPILER_C_STATIC_ASSERT 1
#    else
#      define UTIL_CONFIG_COMPILER_C_STATIC_ASSERT 0
#    endif

#    if (__GNUC__ * 100 + __GNUC_MINOR__) >= 304 && defined(__STDC_VERSION__) && __STDC_VERSION__ >= 199901L
#      define UTIL_CONFIG_COMPILER_C_VARIADIC_MACROS 1
#    else
#      define UTIL_CONFIG_COMPILER_C_VARIADIC_MACROS 0
#    endif

#  elif UTIL_CONFIG_COMPILER_IS_Clang

#    if !(((__clang_major__ * 100) + __clang_minor__) >= 304)
#      error Unsupported compiler version
#    endif

#    define UTIL_CONFIG_COMPILER_VERSION_MAJOR (__clang_major__)
#    define UTIL_CONFIG_COMPILER_VERSION_MINOR (__clang_minor__)
#    define UTIL_CONFIG_COMPILER_VERSION_PATCH (__clang_patchlevel__)
#    if defined(_MSC_VER)
/* _MSC_VER = VVRR */
#      define UTIL_CONFIG_SIMULATE_VERSION_MAJOR (_MSC_VER / 100)
#      define UTIL_CONFIG_SIMULATE_VERSION_MINOR (_MSC_VER % 100)
#    endif

#    if ((__clang_major__ * 100) + __clang_minor__) >= 304 && defined(__STDC_VERSION__) && __STDC_VERSION__ >= 199901L
#      define UTIL_CONFIG_COMPILER_C_RESTRICT 1
#    else
#      define UTIL_CONFIG_COMPILER_C_RESTRICT 0
#    endif

#    if ((__clang_major__ * 100) + __clang_minor__) >= 304 && defined(__STDC_VERSION__) && __STDC_VERSION__ >= 201112L
#      define UTIL_CONFIG_COMPILER_C_STATIC_ASSERT 1
#    else
#      define UTIL_CONFIG_COMPILER_C_STATIC_ASSERT 0
#    endif

#    if ((__clang_major__ * 100) + __clang_minor__) >= 304 && defined(__STDC_VERSION__) && __STDC_VERSION__ >= 199901L
#      define UTIL_CONFIG_COMPILER_C_VARIADIC_MACROS 1
#    else
#      define UTIL_CONFIG_COMPILER_C_VARIADIC_MACROS 0
#    endif

#  elif UTIL_CONFIG_COMPILER_IS_AppleClang

#    if !(((__clang_major__ * 100) + __clang_minor__) >= 400)
#      error Unsupported compiler version
#    endif

#    define UTIL_CONFIG_COMPILER_VERSION_MAJOR (__clang_major__)
#    define UTIL_CONFIG_COMPILER_VERSION_MINOR (__clang_minor__)
#    define UTIL_CONFIG_COMPILER_VERSION_PATCH (__clang_patchlevel__)
#    if defined(_MSC_VER)
/* _MSC_VER = VVRR */
#      define UTIL_CONFIG_SIMULATE_VERSION_MAJOR (_MSC_VER / 100)
#      define UTIL_CONFIG_SIMULATE_VERSION_MINOR (_MSC_VER % 100)
#    endif
#    define UTIL_CONFIG_COMPILER_VERSION_TWEAK (__apple_build_version__)

#    if ((__clang_major__ * 100) + __clang_minor__) >= 400 && defined(__STDC_VERSION__) && __STDC_VERSION__ >= 199901L
#      define UTIL_CONFIG_COMPILER_C_RESTRICT 1
#    else
#      define UTIL_CONFIG_COMPILER_C_RESTRICT 0
#    endif

#    if ((__clang_major__ * 100) + __clang_minor__) >= 400 && defined(__STDC_VERSION__) && __STDC_VERSION__ >= 201112L
#      define UTIL_CONFIG_COMPILER_C_STATIC_ASSERT 1
#    else
#      define UTIL_CONFIG_COMPILER_C_STATIC_ASSERT 0
#    endif

#    if ((__clang_major__ * 100) + __clang_minor__) >= 400 && defined(__STDC_VERSION__) && __STDC_VERSION__ >= 199901L
#      define UTIL_CONFIG_COMPILER_C_VARIADIC_MACROS 1
#    else
#      define UTIL_CONFIG_COMPILER_C_VARIADIC_MACROS 0
#    endif

#  elif UTIL_CONFIG_COMPILER_IS_MSVC

#    if !(_MSC_VER >= 1600)
#      error Unsupported compiler version
#    endif

/* _MSC_VER = VVRR */
#    define UTIL_CONFIG_COMPILER_VERSION_MAJOR (_MSC_VER / 100)
#    define UTIL_CONFIG_COMPILER_VERSION_MINOR (_MSC_VER % 100)
#    if defined(_MSC_FULL_VER)
#      if _MSC_VER >= 1400
/* _MSC_FULL_VER = VVRRPPPPP */
#        define UTIL_CONFIG_COMPILER_VERSION_PATCH (_MSC_FULL_VER % 100000)
#      else
/* _MSC_FULL_VER = VVRRPPPP */
#        define UTIL_CONFIG_COMPILER_VERSION_PATCH (_MSC_FULL_VER % 10000)
#      endif
#    endif
#    if defined(_MSC_BUILD)
#      define UTIL_CONFIG_COMPILER_VERSION_TWEAK (_MSC_BUILD)
#    endif

#    if _MSC_VER >= 1927
#      define UTIL_CONFIG_COMPILER_C_RESTRICT 1
#    else
#      define UTIL_CONFIG_COMPILER_C_RESTRICT 0
#    endif

#    if _MSC_VER >= 1928
#      define UTIL_CONFIG_COMPILER_C_STATIC_ASSERT 1
#    else
#      define UTIL_CONFIG_COMPILER_C_STATIC_ASSERT 0
#    endif

#    if _MSC_VER >= 1600
#      define UTIL_CONFIG_COMPILER_C_VARIADIC_MACROS 1
#    else
#      define UTIL_CONFIG_COMPILER_C_VARIADIC_MACROS 0
#    endif

#  else
#    error Unsupported compiler
#  endif

#  if defined(UTIL_CONFIG_COMPILER_C_RESTRICT) && UTIL_CONFIG_COMPILER_C_RESTRICT
#    define UTIL_CONFIG_RESTRICT restrict
#  else
#    define UTIL_CONFIG_RESTRICT
#  endif

#endif

#ifdef __cplusplus
#  define UTIL_CONFIG_COMPILER_IS_Comeau 0
#  define UTIL_CONFIG_COMPILER_IS_Intel 0
#  define UTIL_CONFIG_COMPILER_IS_PathScale 0
#  define UTIL_CONFIG_COMPILER_IS_Embarcadero 0
#  define UTIL_CONFIG_COMPILER_IS_Borland 0
#  define UTIL_CONFIG_COMPILER_IS_Watcom 0
#  define UTIL_CONFIG_COMPILER_IS_OpenWatcom 0
#  define UTIL_CONFIG_COMPILER_IS_SunPro 0
#  define UTIL_CONFIG_COMPILER_IS_HP 0
#  define UTIL_CONFIG_COMPILER_IS_Compaq 0
#  define UTIL_CONFIG_COMPILER_IS_zOS 0
#  define UTIL_CONFIG_COMPILER_IS_XLClang 0
#  define UTIL_CONFIG_COMPILER_IS_XL 0
#  define UTIL_CONFIG_COMPILER_IS_VisualAge 0
#  define UTIL_CONFIG_COMPILER_IS_PGI 0
#  define UTIL_CONFIG_COMPILER_IS_Cray 0
#  define UTIL_CONFIG_COMPILER_IS_TI 0
#  define UTIL_CONFIG_COMPILER_IS_Fujitsu 0
#  define UTIL_CONFIG_COMPILER_IS_GHS 0
#  define UTIL_CONFIG_COMPILER_IS_SCO 0
#  define UTIL_CONFIG_COMPILER_IS_ARMCC 0
#  define UTIL_CONFIG_COMPILER_IS_AppleClang 0
#  define UTIL_CONFIG_COMPILER_IS_ARMClang 0
#  define UTIL_CONFIG_COMPILER_IS_Clang 0
#  define UTIL_CONFIG_COMPILER_IS_GNU 0
#  define UTIL_CONFIG_COMPILER_IS_MSVC 0
#  define UTIL_CONFIG_COMPILER_IS_ADSP 0
#  define UTIL_CONFIG_COMPILER_IS_IAR 0
#  define UTIL_CONFIG_COMPILER_IS_MIPSpro 0

#  if defined(__COMO__)
#    undef UTIL_CONFIG_COMPILER_IS_Comeau
#    define UTIL_CONFIG_COMPILER_IS_Comeau 1

#  elif defined(__INTEL_COMPILER) || defined(__ICC)
#    undef UTIL_CONFIG_COMPILER_IS_Intel
#    define UTIL_CONFIG_COMPILER_IS_Intel 1

#  elif defined(__PATHCC__)
#    undef UTIL_CONFIG_COMPILER_IS_PathScale
#    define UTIL_CONFIG_COMPILER_IS_PathScale 1

#  elif defined(__BORLANDC__) && defined(__CODEGEARC_VERSION__)
#    undef UTIL_CONFIG_COMPILER_IS_Embarcadero
#    define UTIL_CONFIG_COMPILER_IS_Embarcadero 1

#  elif defined(__BORLANDC__)
#    undef UTIL_CONFIG_COMPILER_IS_Borland
#    define UTIL_CONFIG_COMPILER_IS_Borland 1

#  elif defined(__WATCOMC__) && __WATCOMC__ < 1200
#    undef UTIL_CONFIG_COMPILER_IS_Watcom
#    define UTIL_CONFIG_COMPILER_IS_Watcom 1

#  elif defined(__WATCOMC__)
#    undef UTIL_CONFIG_COMPILER_IS_OpenWatcom
#    define UTIL_CONFIG_COMPILER_IS_OpenWatcom 1

#  elif defined(__SUNPRO_CC)
#    undef UTIL_CONFIG_COMPILER_IS_SunPro
#    define UTIL_CONFIG_COMPILER_IS_SunPro 1

#  elif defined(__HP_aCC)
#    undef UTIL_CONFIG_COMPILER_IS_HP
#    define UTIL_CONFIG_COMPILER_IS_HP 1

#  elif defined(__DECCXX)
#    undef UTIL_CONFIG_COMPILER_IS_Compaq
#    define UTIL_CONFIG_COMPILER_IS_Compaq 1

#  elif defined(__IBMCPP__) && defined(__COMPILER_VER__)
#    undef UTIL_CONFIG_COMPILER_IS_zOS
#    define UTIL_CONFIG_COMPILER_IS_zOS 1

#  elif defined(__ibmxl__) && defined(__clang__)
#    undef UTIL_CONFIG_COMPILER_IS_XLClang
#    define UTIL_CONFIG_COMPILER_IS_XLClang 1

#  elif defined(__IBMCPP__) && !defined(__COMPILER_VER__) && __IBMCPP__ >= 800
#    undef UTIL_CONFIG_COMPILER_IS_XL
#    define UTIL_CONFIG_COMPILER_IS_XL 1

#  elif defined(__IBMCPP__) && !defined(__COMPILER_VER__) && __IBMCPP__ < 800
#    undef UTIL_CONFIG_COMPILER_IS_VisualAge
#    define UTIL_CONFIG_COMPILER_IS_VisualAge 1

#  elif defined(__PGI)
#    undef UTIL_CONFIG_COMPILER_IS_PGI
#    define UTIL_CONFIG_COMPILER_IS_PGI 1

#  elif defined(_CRAYC)
#    undef UTIL_CONFIG_COMPILER_IS_Cray
#    define UTIL_CONFIG_COMPILER_IS_Cray 1

#  elif defined(__TI_COMPILER_VERSION__)
#    undef UTIL_CONFIG_COMPILER_IS_TI
#    define UTIL_CONFIG_COMPILER_IS_TI 1

#  elif defined(__FUJITSU) || defined(__FCC_VERSION) || defined(__fcc_version)
#    undef UTIL_CONFIG_COMPILER_IS_Fujitsu
#    define UTIL_CONFIG_COMPILER_IS_Fujitsu 1

#  elif defined(__ghs__)
#    undef UTIL_CONFIG_COMPILER_IS_GHS
#    define UTIL_CONFIG_COMPILER_IS_GHS 1

#  elif defined(__SCO_VERSION__)
#    undef UTIL_CONFIG_COMPILER_IS_SCO
#    define UTIL_CONFIG_COMPILER_IS_SCO 1

#  elif defined(__ARMCC_VERSION) && !defined(__clang__)
#    undef UTIL_CONFIG_COMPILER_IS_ARMCC
#    define UTIL_CONFIG_COMPILER_IS_ARMCC 1

#  elif defined(__clang__) && defined(__apple_build_version__)
#    undef UTIL_CONFIG_COMPILER_IS_AppleClang
#    define UTIL_CONFIG_COMPILER_IS_AppleClang 1

#  elif defined(__clang__) && defined(__ARMCOMPILER_VERSION)
#    undef UTIL_CONFIG_COMPILER_IS_ARMClang
#    define UTIL_CONFIG_COMPILER_IS_ARMClang 1

#  elif defined(__clang__)
#    undef UTIL_CONFIG_COMPILER_IS_Clang
#    define UTIL_CONFIG_COMPILER_IS_Clang 1

#  elif defined(__GNUC__) || defined(__GNUG__)
#    undef UTIL_CONFIG_COMPILER_IS_GNU
#    define UTIL_CONFIG_COMPILER_IS_GNU 1

#  elif defined(_MSC_VER)
#    undef UTIL_CONFIG_COMPILER_IS_MSVC
#    define UTIL_CONFIG_COMPILER_IS_MSVC 1

#  elif defined(__VISUALDSPVERSION__) || defined(__ADSPBLACKFIN__) || defined(__ADSPTS__) || defined(__ADSP21000__)
#    undef UTIL_CONFIG_COMPILER_IS_ADSP
#    define UTIL_CONFIG_COMPILER_IS_ADSP 1

#  elif defined(__IAR_SYSTEMS_ICC__) || defined(__IAR_SYSTEMS_ICC)
#    undef UTIL_CONFIG_COMPILER_IS_IAR
#    define UTIL_CONFIG_COMPILER_IS_IAR 1

#  endif

#  if UTIL_CONFIG_COMPILER_IS_GNU

#    if !((__GNUC__ * 100 + __GNUC_MINOR__) >= 404)
#      error Unsupported compiler version
#    endif

#    if defined(__GNUC__)
#      define UTIL_CONFIG_COMPILER_VERSION_MAJOR (__GNUC__)
#    else
#      define UTIL_CONFIG_COMPILER_VERSION_MAJOR (__GNUG__)
#    endif
#    if defined(__GNUC_MINOR__)
#      define UTIL_CONFIG_COMPILER_VERSION_MINOR (__GNUC_MINOR__)
#    endif
#    if defined(__GNUC_PATCHLEVEL__)
#      define UTIL_CONFIG_COMPILER_VERSION_PATCH (__GNUC_PATCHLEVEL__)
#    endif

#    if (__GNUC__ * 100 + __GNUC_MINOR__) >= 407 && __cplusplus >= 201103L
#      define UTIL_CONFIG_COMPILER_CXX_ALIAS_TEMPLATES 1
#    else
#      define UTIL_CONFIG_COMPILER_CXX_ALIAS_TEMPLATES 0
#    endif

#    if (__GNUC__ * 100 + __GNUC_MINOR__) >= 408 && __cplusplus >= 201103L
#      define UTIL_CONFIG_COMPILER_CXX_ATTRIBUTES 1
#    else
#      define UTIL_CONFIG_COMPILER_CXX_ATTRIBUTES 0
#    endif

#    if (__GNUC__ * 100 + __GNUC_MINOR__) >= 409 && __cplusplus > 201103L
#      define UTIL_CONFIG_COMPILER_CXX_ATTRIBUTE_DEPRECATED 1
#    else
#      define UTIL_CONFIG_COMPILER_CXX_ATTRIBUTE_DEPRECATED 0
#    endif

#    if (__GNUC__ * 100 + __GNUC_MINOR__) >= 404 && \
        (__cplusplus >= 201103L || (defined(__GXX_EXPERIMENTAL_CXX0X__) && __GXX_EXPERIMENTAL_CXX0X__))
#      define UTIL_CONFIG_COMPILER_CXX_AUTO_TYPE 1
#    else
#      define UTIL_CONFIG_COMPILER_CXX_AUTO_TYPE 0
#    endif

#    if (__GNUC__ * 100 + __GNUC_MINOR__) >= 406 && \
        (__cplusplus >= 201103L || (defined(__GXX_EXPERIMENTAL_CXX0X__) && __GXX_EXPERIMENTAL_CXX0X__))
#      define UTIL_CONFIG_COMPILER_CXX_CONSTEXPR 1
#    else
#      define UTIL_CONFIG_COMPILER_CXX_CONSTEXPR 0
#    endif

#    if (__GNUC__ * 100 + __GNUC_MINOR__) >= 404 && \
        (__cplusplus >= 201103L || (defined(__GXX_EXPERIMENTAL_CXX0X__) && __GXX_EXPERIMENTAL_CXX0X__))
#      define UTIL_CONFIG_COMPILER_CXX_DECLTYPE 1
#    else
#      define UTIL_CONFIG_COMPILER_CXX_DECLTYPE 0
#    endif

#    if (__GNUC__ * 100 + __GNUC_MINOR__) >= 409 && __cplusplus > 201103L
#      define UTIL_CONFIG_COMPILER_CXX_DECLTYPE_AUTO 1
#    else
#      define UTIL_CONFIG_COMPILER_CXX_DECLTYPE_AUTO 0
#    endif

#    if (__GNUC__ * 100 + __GNUC_MINOR__) >= 404 && \
        (__cplusplus >= 201103L || (defined(__GXX_EXPERIMENTAL_CXX0X__) && __GXX_EXPERIMENTAL_CXX0X__))
#      define UTIL_CONFIG_COMPILER_CXX_DEFAULT_FUNCTION_TEMPLATE_ARGS 1
#    else
#      define UTIL_CONFIG_COMPILER_CXX_DEFAULT_FUNCTION_TEMPLATE_ARGS 0
#    endif

#    if (__GNUC__ * 100 + __GNUC_MINOR__) >= 404 && \
        (__cplusplus >= 201103L || (defined(__GXX_EXPERIMENTAL_CXX0X__) && __GXX_EXPERIMENTAL_CXX0X__))
#      define UTIL_CONFIG_COMPILER_CXX_DEFAULTED_FUNCTIONS 1
#    else
#      define UTIL_CONFIG_COMPILER_CXX_DEFAULTED_FUNCTIONS 0
#    endif

#    if (__GNUC__ * 100 + __GNUC_MINOR__) >= 407 && __cplusplus >= 201103L
#      define UTIL_CONFIG_COMPILER_CXX_DELEGATING_CONSTRUCTORS 1
#    else
#      define UTIL_CONFIG_COMPILER_CXX_DELEGATING_CONSTRUCTORS 0
#    endif

#    if (__GNUC__ * 100 + __GNUC_MINOR__) >= 404 && \
        (__cplusplus >= 201103L || (defined(__GXX_EXPERIMENTAL_CXX0X__) && __GXX_EXPERIMENTAL_CXX0X__))
#      define UTIL_CONFIG_COMPILER_CXX_DELETED_FUNCTIONS 1
#    else
#      define UTIL_CONFIG_COMPILER_CXX_DELETED_FUNCTIONS 0
#    endif

#    if (__GNUC__ * 100 + __GNUC_MINOR__) >= 407 && __cplusplus >= 201103L
#      define UTIL_CONFIG_COMPILER_CXX_FINAL 1
#    else
#      define UTIL_CONFIG_COMPILER_CXX_FINAL 0
#    endif

#    if (__GNUC__ * 100 + __GNUC_MINOR__) >= 409 && __cplusplus > 201103L
#      define UTIL_CONFIG_COMPILER_CXX_GENERIC_LAMBDAS 1
#    else
#      define UTIL_CONFIG_COMPILER_CXX_GENERIC_LAMBDAS 0
#    endif

#    if (__GNUC__ * 100 + __GNUC_MINOR__) >= 408 && __cplusplus >= 201103L
#      define UTIL_CONFIG_COMPILER_CXX_INHERITING_CONSTRUCTORS 1
#    else
#      define UTIL_CONFIG_COMPILER_CXX_INHERITING_CONSTRUCTORS 0
#    endif

#    if (__GNUC__ * 100 + __GNUC_MINOR__) >= 405 && \
        (__cplusplus >= 201103L || (defined(__GXX_EXPERIMENTAL_CXX0X__) && __GXX_EXPERIMENTAL_CXX0X__))
#      define UTIL_CONFIG_COMPILER_CXX_LAMBDAS 1
#    else
#      define UTIL_CONFIG_COMPILER_CXX_LAMBDAS 0
#    endif

#    if (__GNUC__ * 100 + __GNUC_MINOR__) >= 404 && \
        (__cplusplus >= 201103L || (defined(__GXX_EXPERIMENTAL_CXX0X__) && __GXX_EXPERIMENTAL_CXX0X__))
#      define UTIL_CONFIG_COMPILER_CXX_LONG_LONG_TYPE 1
#    else
#      define UTIL_CONFIG_COMPILER_CXX_LONG_LONG_TYPE 0
#    endif

#    if (__GNUC__ * 100 + __GNUC_MINOR__) >= 406 && \
        (__cplusplus >= 201103L || (defined(__GXX_EXPERIMENTAL_CXX0X__) && __GXX_EXPERIMENTAL_CXX0X__))
#      define UTIL_CONFIG_COMPILER_CXX_NOEXCEPT 1
#    else
#      define UTIL_CONFIG_COMPILER_CXX_NOEXCEPT 0
#    endif

#    if (__GNUC__ * 100 + __GNUC_MINOR__) >= 407 && __cplusplus >= 201103L
#      define UTIL_CONFIG_COMPILER_CXX_NONSTATIC_MEMBER_INIT 1
#    else
#      define UTIL_CONFIG_COMPILER_CXX_NONSTATIC_MEMBER_INIT 0
#    endif

#    if (__GNUC__ * 100 + __GNUC_MINOR__) >= 406 && \
        (__cplusplus >= 201103L || (defined(__GXX_EXPERIMENTAL_CXX0X__) && __GXX_EXPERIMENTAL_CXX0X__))
#      define UTIL_CONFIG_COMPILER_CXX_NULLPTR 1
#    else
#      define UTIL_CONFIG_COMPILER_CXX_NULLPTR 0
#    endif

#    if (__GNUC__ * 100 + __GNUC_MINOR__) >= 407 && __cplusplus >= 201103L
#      define UTIL_CONFIG_COMPILER_CXX_OVERRIDE 1
#    else
#      define UTIL_CONFIG_COMPILER_CXX_OVERRIDE 0
#    endif

#    if (__GNUC__ * 100 + __GNUC_MINOR__) >= 406 && \
        (__cplusplus >= 201103L || (defined(__GXX_EXPERIMENTAL_CXX0X__) && __GXX_EXPERIMENTAL_CXX0X__))
#      define UTIL_CONFIG_COMPILER_CXX_RANGE_FOR 1
#    else
#      define UTIL_CONFIG_COMPILER_CXX_RANGE_FOR 0
#    endif

#    if (__GNUC__ * 100 + __GNUC_MINOR__) >= 405 && \
        (__cplusplus >= 201103L || (defined(__GXX_EXPERIMENTAL_CXX0X__) && __GXX_EXPERIMENTAL_CXX0X__))
#      define UTIL_CONFIG_COMPILER_CXX_RAW_STRING_LITERALS 1
#    else
#      define UTIL_CONFIG_COMPILER_CXX_RAW_STRING_LITERALS 0
#    endif

#    if (__GNUC__ * 100 + __GNUC_MINOR__) >= 500 && __cplusplus >= 201402L
#      define UTIL_CONFIG_COMPILER_CXX_RELAXED_CONSTEXPR 1
#    else
#      define UTIL_CONFIG_COMPILER_CXX_RELAXED_CONSTEXPR 0
#    endif

#    if (__GNUC__ * 100 + __GNUC_MINOR__) >= 409 && __cplusplus > 201103L
#      define UTIL_CONFIG_COMPILER_CXX_RETURN_TYPE_DEDUCTION 1
#    else
#      define UTIL_CONFIG_COMPILER_CXX_RETURN_TYPE_DEDUCTION 0
#    endif

#    if (__GNUC__ * 100 + __GNUC_MINOR__) >= 404 && \
        (__cplusplus >= 201103L || (defined(__GXX_EXPERIMENTAL_CXX0X__) && __GXX_EXPERIMENTAL_CXX0X__))
#      define UTIL_CONFIG_COMPILER_CXX_RVALUE_REFERENCES 1
#    else
#      define UTIL_CONFIG_COMPILER_CXX_RVALUE_REFERENCES 0
#    endif

#    if (__GNUC__ * 100 + __GNUC_MINOR__) >= 404 && \
        (__cplusplus >= 201103L || (defined(__GXX_EXPERIMENTAL_CXX0X__) && __GXX_EXPERIMENTAL_CXX0X__))
#      define UTIL_CONFIG_COMPILER_CXX_SIZEOF_MEMBER 1
#    else
#      define UTIL_CONFIG_COMPILER_CXX_SIZEOF_MEMBER 0
#    endif

#    if (__GNUC__ * 100 + __GNUC_MINOR__) >= 404 && \
        (__cplusplus >= 201103L || (defined(__GXX_EXPERIMENTAL_CXX0X__) && __GXX_EXPERIMENTAL_CXX0X__))
#      define UTIL_CONFIG_COMPILER_CXX_STATIC_ASSERT 1
#    else
#      define UTIL_CONFIG_COMPILER_CXX_STATIC_ASSERT 0
#    endif

#    if (__GNUC__ * 100 + __GNUC_MINOR__) >= 408 && __cplusplus >= 201103L
#      define UTIL_CONFIG_COMPILER_CXX_THREAD_LOCAL 1
#    else
#      define UTIL_CONFIG_COMPILER_CXX_THREAD_LOCAL 0
#    endif

#    if (__GNUC__ * 100 + __GNUC_MINOR__) >= 404 && \
        (__cplusplus >= 201103L || (defined(__GXX_EXPERIMENTAL_CXX0X__) && __GXX_EXPERIMENTAL_CXX0X__))
#      define UTIL_CONFIG_COMPILER_CXX_VARIADIC_TEMPLATES 1
#    else
#      define UTIL_CONFIG_COMPILER_CXX_VARIADIC_TEMPLATES 0
#    endif

#  elif UTIL_CONFIG_COMPILER_IS_Clang

#    if !(((__clang_major__ * 100) + __clang_minor__) >= 301)
#      error Unsupported compiler version
#    endif

#    define UTIL_CONFIG_COMPILER_VERSION_MAJOR (__clang_major__)
#    define UTIL_CONFIG_COMPILER_VERSION_MINOR (__clang_minor__)
#    define UTIL_CONFIG_COMPILER_VERSION_PATCH (__clang_patchlevel__)
#    if defined(_MSC_VER)
/* _MSC_VER = VVRR */
#      define UTIL_CONFIG_SIMULATE_VERSION_MAJOR (_MSC_VER / 100)
#      define UTIL_CONFIG_SIMULATE_VERSION_MINOR (_MSC_VER % 100)
#    endif

#    if ((__clang_major__ * 100) + __clang_minor__) >= 301 && __has_feature(cxx_alias_templates)
#      define UTIL_CONFIG_COMPILER_CXX_ALIAS_TEMPLATES 1
#    else
#      define UTIL_CONFIG_COMPILER_CXX_ALIAS_TEMPLATES 0
#    endif

#    if ((__clang_major__ * 100) + __clang_minor__) >= 301 && __has_feature(cxx_attributes)
#      define UTIL_CONFIG_COMPILER_CXX_ATTRIBUTES 1
#    else
#      define UTIL_CONFIG_COMPILER_CXX_ATTRIBUTES 0
#    endif

#    if ((__clang_major__ * 100) + __clang_minor__) >= 304 && __cplusplus > 201103L
#      define UTIL_CONFIG_COMPILER_CXX_ATTRIBUTE_DEPRECATED 1
#    else
#      define UTIL_CONFIG_COMPILER_CXX_ATTRIBUTE_DEPRECATED 0
#    endif

#    if ((__clang_major__ * 100) + __clang_minor__) >= 301 && __has_feature(cxx_auto_type)
#      define UTIL_CONFIG_COMPILER_CXX_AUTO_TYPE 1
#    else
#      define UTIL_CONFIG_COMPILER_CXX_AUTO_TYPE 0
#    endif

#    if ((__clang_major__ * 100) + __clang_minor__) >= 301 && __has_feature(cxx_constexpr)
#      define UTIL_CONFIG_COMPILER_CXX_CONSTEXPR 1
#    else
#      define UTIL_CONFIG_COMPILER_CXX_CONSTEXPR 0
#    endif

#    if ((__clang_major__ * 100) + __clang_minor__) >= 301 && __has_feature(cxx_decltype)
#      define UTIL_CONFIG_COMPILER_CXX_DECLTYPE 1
#    else
#      define UTIL_CONFIG_COMPILER_CXX_DECLTYPE 0
#    endif

#    if ((__clang_major__ * 100) + __clang_minor__) >= 304 && __cplusplus > 201103L
#      define UTIL_CONFIG_COMPILER_CXX_DECLTYPE_AUTO 1
#    else
#      define UTIL_CONFIG_COMPILER_CXX_DECLTYPE_AUTO 0
#    endif

#    if ((__clang_major__ * 100) + __clang_minor__) >= 301 && __has_feature(cxx_default_function_template_args)
#      define UTIL_CONFIG_COMPILER_CXX_DEFAULT_FUNCTION_TEMPLATE_ARGS 1
#    else
#      define UTIL_CONFIG_COMPILER_CXX_DEFAULT_FUNCTION_TEMPLATE_ARGS 0
#    endif

#    if ((__clang_major__ * 100) + __clang_minor__) >= 301 && __has_feature(cxx_defaulted_functions)
#      define UTIL_CONFIG_COMPILER_CXX_DEFAULTED_FUNCTIONS 1
#    else
#      define UTIL_CONFIG_COMPILER_CXX_DEFAULTED_FUNCTIONS 0
#    endif

#    if ((__clang_major__ * 100) + __clang_minor__) >= 301 && __has_feature(cxx_delegating_constructors)
#      define UTIL_CONFIG_COMPILER_CXX_DELEGATING_CONSTRUCTORS 1
#    else
#      define UTIL_CONFIG_COMPILER_CXX_DELEGATING_CONSTRUCTORS 0
#    endif

#    if ((__clang_major__ * 100) + __clang_minor__) >= 301 && __has_feature(cxx_deleted_functions)
#      define UTIL_CONFIG_COMPILER_CXX_DELETED_FUNCTIONS 1
#    else
#      define UTIL_CONFIG_COMPILER_CXX_DELETED_FUNCTIONS 0
#    endif

#    if ((__clang_major__ * 100) + __clang_minor__) >= 301 && __has_feature(cxx_override_control)
#      define UTIL_CONFIG_COMPILER_CXX_FINAL 1
#    else
#      define UTIL_CONFIG_COMPILER_CXX_FINAL 0
#    endif

#    if ((__clang_major__ * 100) + __clang_minor__) >= 304 && __cplusplus > 201103L
#      define UTIL_CONFIG_COMPILER_CXX_GENERIC_LAMBDAS 1
#    else
#      define UTIL_CONFIG_COMPILER_CXX_GENERIC_LAMBDAS 0
#    endif

#    if ((__clang_major__ * 100) + __clang_minor__) >= 301 && __has_feature(cxx_inheriting_constructors)
#      define UTIL_CONFIG_COMPILER_CXX_INHERITING_CONSTRUCTORS 1
#    else
#      define UTIL_CONFIG_COMPILER_CXX_INHERITING_CONSTRUCTORS 0
#    endif

#    if ((__clang_major__ * 100) + __clang_minor__) >= 301 && __has_feature(cxx_lambdas)
#      define UTIL_CONFIG_COMPILER_CXX_LAMBDAS 1
#    else
#      define UTIL_CONFIG_COMPILER_CXX_LAMBDAS 0
#    endif

#    if ((__clang_major__ * 100) + __clang_minor__) >= 301 && __cplusplus >= 201103L
#      define UTIL_CONFIG_COMPILER_CXX_LONG_LONG_TYPE 1
#    else
#      define UTIL_CONFIG_COMPILER_CXX_LONG_LONG_TYPE 0
#    endif

#    if ((__clang_major__ * 100) + __clang_minor__) >= 301 && __has_feature(cxx_noexcept)
#      define UTIL_CONFIG_COMPILER_CXX_NOEXCEPT 1
#    else
#      define UTIL_CONFIG_COMPILER_CXX_NOEXCEPT 0
#    endif

#    if ((__clang_major__ * 100) + __clang_minor__) >= 301 && __has_feature(cxx_nonstatic_member_init)
#      define UTIL_CONFIG_COMPILER_CXX_NONSTATIC_MEMBER_INIT 1
#    else
#      define UTIL_CONFIG_COMPILER_CXX_NONSTATIC_MEMBER_INIT 0
#    endif

#    if ((__clang_major__ * 100) + __clang_minor__) >= 301 && __has_feature(cxx_nullptr)
#      define UTIL_CONFIG_COMPILER_CXX_NULLPTR 1
#    else
#      define UTIL_CONFIG_COMPILER_CXX_NULLPTR 0
#    endif

#    if ((__clang_major__ * 100) + __clang_minor__) >= 301 && __has_feature(cxx_override_control)
#      define UTIL_CONFIG_COMPILER_CXX_OVERRIDE 1
#    else
#      define UTIL_CONFIG_COMPILER_CXX_OVERRIDE 0
#    endif

#    if ((__clang_major__ * 100) + __clang_minor__) >= 301 && __has_feature(cxx_range_for)
#      define UTIL_CONFIG_COMPILER_CXX_RANGE_FOR 1
#    else
#      define UTIL_CONFIG_COMPILER_CXX_RANGE_FOR 0
#    endif

#    if ((__clang_major__ * 100) + __clang_minor__) >= 301 && __has_feature(cxx_raw_string_literals)
#      define UTIL_CONFIG_COMPILER_CXX_RAW_STRING_LITERALS 1
#    else
#      define UTIL_CONFIG_COMPILER_CXX_RAW_STRING_LITERALS 0
#    endif

#    if ((__clang_major__ * 100) + __clang_minor__) >= 301 && __has_feature(cxx_relaxed_constexpr)
#      define UTIL_CONFIG_COMPILER_CXX_RELAXED_CONSTEXPR 1
#    else
#      define UTIL_CONFIG_COMPILER_CXX_RELAXED_CONSTEXPR 0
#    endif

#    if ((__clang_major__ * 100) + __clang_minor__) >= 301 && __has_feature(cxx_return_type_deduction)
#      define UTIL_CONFIG_COMPILER_CXX_RETURN_TYPE_DEDUCTION 1
#    else
#      define UTIL_CONFIG_COMPILER_CXX_RETURN_TYPE_DEDUCTION 0
#    endif

#    if ((__clang_major__ * 100) + __clang_minor__) >= 301 && __has_feature(cxx_rvalue_references)
#      define UTIL_CONFIG_COMPILER_CXX_RVALUE_REFERENCES 1
#    else
#      define UTIL_CONFIG_COMPILER_CXX_RVALUE_REFERENCES 0
#    endif

#    if ((__clang_major__ * 100) + __clang_minor__) >= 301 && __cplusplus >= 201103L
#      define UTIL_CONFIG_COMPILER_CXX_SIZEOF_MEMBER 1
#    else
#      define UTIL_CONFIG_COMPILER_CXX_SIZEOF_MEMBER 0
#    endif

#    if ((__clang_major__ * 100) + __clang_minor__) >= 301 && __has_feature(cxx_static_assert)
#      define UTIL_CONFIG_COMPILER_CXX_STATIC_ASSERT 1
#    else
#      define UTIL_CONFIG_COMPILER_CXX_STATIC_ASSERT 0
#    endif

#    if ((__clang_major__ * 100) + __clang_minor__) >= 301 && __has_feature(cxx_thread_local)
#      define UTIL_CONFIG_COMPILER_CXX_THREAD_LOCAL 1
#    else
#      define UTIL_CONFIG_COMPILER_CXX_THREAD_LOCAL 0
#    endif

#    if ((__clang_major__ * 100) + __clang_minor__) >= 301 && __has_feature(cxx_variadic_templates)
#      define UTIL_CONFIG_COMPILER_CXX_VARIADIC_TEMPLATES 1
#    else
#      define UTIL_CONFIG_COMPILER_CXX_VARIADIC_TEMPLATES 0
#    endif

#  elif UTIL_CONFIG_COMPILER_IS_AppleClang

#    if !(((__clang_major__ * 100) + __clang_minor__) >= 400)
#      error Unsupported compiler version
#    endif

#    define UTIL_CONFIG_COMPILER_VERSION_MAJOR (__clang_major__)
#    define UTIL_CONFIG_COMPILER_VERSION_MINOR (__clang_minor__)
#    define UTIL_CONFIG_COMPILER_VERSION_PATCH (__clang_patchlevel__)
#    if defined(_MSC_VER)
/* _MSC_VER = VVRR */
#      define UTIL_CONFIG_SIMULATE_VERSION_MAJOR (_MSC_VER / 100)
#      define UTIL_CONFIG_SIMULATE_VERSION_MINOR (_MSC_VER % 100)
#    endif
#    define UTIL_CONFIG_COMPILER_VERSION_TWEAK (__apple_build_version__)

#    if ((__clang_major__ * 100) + __clang_minor__) >= 400 && __has_feature(cxx_alias_templates)
#      define UTIL_CONFIG_COMPILER_CXX_ALIAS_TEMPLATES 1
#    else
#      define UTIL_CONFIG_COMPILER_CXX_ALIAS_TEMPLATES 0
#    endif

#    if ((__clang_major__ * 100) + __clang_minor__) >= 400 && __has_feature(cxx_attributes)
#      define UTIL_CONFIG_COMPILER_CXX_ATTRIBUTES 1
#    else
#      define UTIL_CONFIG_COMPILER_CXX_ATTRIBUTES 0
#    endif

#    if ((__clang_major__ * 100) + __clang_minor__) >= 501 && __cplusplus > 201103L
#      define UTIL_CONFIG_COMPILER_CXX_ATTRIBUTE_DEPRECATED 1
#    else
#      define UTIL_CONFIG_COMPILER_CXX_ATTRIBUTE_DEPRECATED 0
#    endif

#    if ((__clang_major__ * 100) + __clang_minor__) >= 400 && __has_feature(cxx_auto_type)
#      define UTIL_CONFIG_COMPILER_CXX_AUTO_TYPE 1
#    else
#      define UTIL_CONFIG_COMPILER_CXX_AUTO_TYPE 0
#    endif

#    if ((__clang_major__ * 100) + __clang_minor__) >= 400 && __has_feature(cxx_constexpr)
#      define UTIL_CONFIG_COMPILER_CXX_CONSTEXPR 1
#    else
#      define UTIL_CONFIG_COMPILER_CXX_CONSTEXPR 0
#    endif

#    if ((__clang_major__ * 100) + __clang_minor__) >= 400 && __has_feature(cxx_decltype)
#      define UTIL_CONFIG_COMPILER_CXX_DECLTYPE 1
#    else
#      define UTIL_CONFIG_COMPILER_CXX_DECLTYPE 0
#    endif

#    if ((__clang_major__ * 100) + __clang_minor__) >= 501 && __cplusplus > 201103L
#      define UTIL_CONFIG_COMPILER_CXX_DECLTYPE_AUTO 1
#    else
#      define UTIL_CONFIG_COMPILER_CXX_DECLTYPE_AUTO 0
#    endif

#    if ((__clang_major__ * 100) + __clang_minor__) >= 400 && __has_feature(cxx_default_function_template_args)
#      define UTIL_CONFIG_COMPILER_CXX_DEFAULT_FUNCTION_TEMPLATE_ARGS 1
#    else
#      define UTIL_CONFIG_COMPILER_CXX_DEFAULT_FUNCTION_TEMPLATE_ARGS 0
#    endif

#    if ((__clang_major__ * 100) + __clang_minor__) >= 400 && __has_feature(cxx_defaulted_functions)
#      define UTIL_CONFIG_COMPILER_CXX_DEFAULTED_FUNCTIONS 1
#    else
#      define UTIL_CONFIG_COMPILER_CXX_DEFAULTED_FUNCTIONS 0
#    endif

#    if ((__clang_major__ * 100) + __clang_minor__) >= 400 && __has_feature(cxx_delegating_constructors)
#      define UTIL_CONFIG_COMPILER_CXX_DELEGATING_CONSTRUCTORS 1
#    else
#      define UTIL_CONFIG_COMPILER_CXX_DELEGATING_CONSTRUCTORS 0
#    endif

#    if ((__clang_major__ * 100) + __clang_minor__) >= 400 && __has_feature(cxx_deleted_functions)
#      define UTIL_CONFIG_COMPILER_CXX_DELETED_FUNCTIONS 1
#    else
#      define UTIL_CONFIG_COMPILER_CXX_DELETED_FUNCTIONS 0
#    endif

#    if ((__clang_major__ * 100) + __clang_minor__) >= 400 && __has_feature(cxx_override_control)
#      define UTIL_CONFIG_COMPILER_CXX_FINAL 1
#    else
#      define UTIL_CONFIG_COMPILER_CXX_FINAL 0
#    endif

#    if ((__clang_major__ * 100) + __clang_minor__) >= 501 && __cplusplus > 201103L
#      define UTIL_CONFIG_COMPILER_CXX_GENERIC_LAMBDAS 1
#    else
#      define UTIL_CONFIG_COMPILER_CXX_GENERIC_LAMBDAS 0
#    endif

#    if ((__clang_major__ * 100) + __clang_minor__) >= 400 && __has_feature(cxx_inheriting_constructors)
#      define UTIL_CONFIG_COMPILER_CXX_INHERITING_CONSTRUCTORS 1
#    else
#      define UTIL_CONFIG_COMPILER_CXX_INHERITING_CONSTRUCTORS 0
#    endif

#    if ((__clang_major__ * 100) + __clang_minor__) >= 400 && __has_feature(cxx_lambdas)
#      define UTIL_CONFIG_COMPILER_CXX_LAMBDAS 1
#    else
#      define UTIL_CONFIG_COMPILER_CXX_LAMBDAS 0
#    endif

#    if ((__clang_major__ * 100) + __clang_minor__) >= 400 && __cplusplus >= 201103L
#      define UTIL_CONFIG_COMPILER_CXX_LONG_LONG_TYPE 1
#    else
#      define UTIL_CONFIG_COMPILER_CXX_LONG_LONG_TYPE 0
#    endif

#    if ((__clang_major__ * 100) + __clang_minor__) >= 400 && __has_feature(cxx_noexcept)
#      define UTIL_CONFIG_COMPILER_CXX_NOEXCEPT 1
#    else
#      define UTIL_CONFIG_COMPILER_CXX_NOEXCEPT 0
#    endif

#    if ((__clang_major__ * 100) + __clang_minor__) >= 400 && __has_feature(cxx_nonstatic_member_init)
#      define UTIL_CONFIG_COMPILER_CXX_NONSTATIC_MEMBER_INIT 1
#    else
#      define UTIL_CONFIG_COMPILER_CXX_NONSTATIC_MEMBER_INIT 0
#    endif

#    if ((__clang_major__ * 100) + __clang_minor__) >= 400 && __has_feature(cxx_nullptr)
#      define UTIL_CONFIG_COMPILER_CXX_NULLPTR 1
#    else
#      define UTIL_CONFIG_COMPILER_CXX_NULLPTR 0
#    endif

#    if ((__clang_major__ * 100) + __clang_minor__) >= 400 && __has_feature(cxx_override_control)
#      define UTIL_CONFIG_COMPILER_CXX_OVERRIDE 1
#    else
#      define UTIL_CONFIG_COMPILER_CXX_OVERRIDE 0
#    endif

#    if ((__clang_major__ * 100) + __clang_minor__) >= 400 && __has_feature(cxx_range_for)
#      define UTIL_CONFIG_COMPILER_CXX_RANGE_FOR 1
#    else
#      define UTIL_CONFIG_COMPILER_CXX_RANGE_FOR 0
#    endif

#    if ((__clang_major__ * 100) + __clang_minor__) >= 400 && __has_feature(cxx_raw_string_literals)
#      define UTIL_CONFIG_COMPILER_CXX_RAW_STRING_LITERALS 1
#    else
#      define UTIL_CONFIG_COMPILER_CXX_RAW_STRING_LITERALS 0
#    endif

#    if ((__clang_major__ * 100) + __clang_minor__) >= 400 && __has_feature(cxx_relaxed_constexpr)
#      define UTIL_CONFIG_COMPILER_CXX_RELAXED_CONSTEXPR 1
#    else
#      define UTIL_CONFIG_COMPILER_CXX_RELAXED_CONSTEXPR 0
#    endif

#    if ((__clang_major__ * 100) + __clang_minor__) >= 400 && __has_feature(cxx_return_type_deduction)
#      define UTIL_CONFIG_COMPILER_CXX_RETURN_TYPE_DEDUCTION 1
#    else
#      define UTIL_CONFIG_COMPILER_CXX_RETURN_TYPE_DEDUCTION 0
#    endif

#    if ((__clang_major__ * 100) + __clang_minor__) >= 400 && __has_feature(cxx_rvalue_references)
#      define UTIL_CONFIG_COMPILER_CXX_RVALUE_REFERENCES 1
#    else
#      define UTIL_CONFIG_COMPILER_CXX_RVALUE_REFERENCES 0
#    endif

#    if ((__clang_major__ * 100) + __clang_minor__) >= 400 && __cplusplus >= 201103L
#      define UTIL_CONFIG_COMPILER_CXX_SIZEOF_MEMBER 1
#    else
#      define UTIL_CONFIG_COMPILER_CXX_SIZEOF_MEMBER 0
#    endif

#    if ((__clang_major__ * 100) + __clang_minor__) >= 400 && __has_feature(cxx_static_assert)
#      define UTIL_CONFIG_COMPILER_CXX_STATIC_ASSERT 1
#    else
#      define UTIL_CONFIG_COMPILER_CXX_STATIC_ASSERT 0
#    endif

#    if ((__clang_major__ * 100) + __clang_minor__) >= 400 && __has_feature(cxx_thread_local)
#      define UTIL_CONFIG_COMPILER_CXX_THREAD_LOCAL 1
#    else
#      define UTIL_CONFIG_COMPILER_CXX_THREAD_LOCAL 0
#    endif

#    if ((__clang_major__ * 100) + __clang_minor__) >= 400 && __has_feature(cxx_variadic_templates)
#      define UTIL_CONFIG_COMPILER_CXX_VARIADIC_TEMPLATES 1
#    else
#      define UTIL_CONFIG_COMPILER_CXX_VARIADIC_TEMPLATES 0
#    endif

#  elif UTIL_CONFIG_COMPILER_IS_MSVC

#    if !(_MSC_VER >= 1600)
#      error Unsupported compiler version
#    endif

/* _MSC_VER = VVRR */
#    define UTIL_CONFIG_COMPILER_VERSION_MAJOR (_MSC_VER / 100)
#    define UTIL_CONFIG_COMPILER_VERSION_MINOR (_MSC_VER % 100)
#    if defined(_MSC_FULL_VER)
#      if _MSC_VER >= 1400
/* _MSC_FULL_VER = VVRRPPPPP */
#        define UTIL_CONFIG_COMPILER_VERSION_PATCH (_MSC_FULL_VER % 100000)
#      else
/* _MSC_FULL_VER = VVRRPPPP */
#        define UTIL_CONFIG_COMPILER_VERSION_PATCH (_MSC_FULL_VER % 10000)
#      endif
#    endif
#    if defined(_MSC_BUILD)
#      define UTIL_CONFIG_COMPILER_VERSION_TWEAK (_MSC_BUILD)
#    endif

#    if _MSC_VER >= 1800
#      define UTIL_CONFIG_COMPILER_CXX_ALIAS_TEMPLATES 1
#    else
#      define UTIL_CONFIG_COMPILER_CXX_ALIAS_TEMPLATES 0
#    endif

#    if _MSC_VER >= 1900
#      define UTIL_CONFIG_COMPILER_CXX_ATTRIBUTES 1
#    else
#      define UTIL_CONFIG_COMPILER_CXX_ATTRIBUTES 0
#    endif

#    if _MSC_VER >= 1900
#      define UTIL_CONFIG_COMPILER_CXX_ATTRIBUTE_DEPRECATED 1
#    else
#      define UTIL_CONFIG_COMPILER_CXX_ATTRIBUTE_DEPRECATED 0
#    endif

#    if _MSC_VER >= 1600
#      define UTIL_CONFIG_COMPILER_CXX_AUTO_TYPE 1
#    else
#      define UTIL_CONFIG_COMPILER_CXX_AUTO_TYPE 0
#    endif

#    if _MSC_VER >= 1900
#      define UTIL_CONFIG_COMPILER_CXX_CONSTEXPR 1
#    else
#      define UTIL_CONFIG_COMPILER_CXX_CONSTEXPR 0
#    endif

#    if _MSC_VER >= 1600
#      define UTIL_CONFIG_COMPILER_CXX_DECLTYPE 1
#    else
#      define UTIL_CONFIG_COMPILER_CXX_DECLTYPE 0
#    endif

#    if _MSC_VER >= 1900
#      define UTIL_CONFIG_COMPILER_CXX_DECLTYPE_AUTO 1
#    else
#      define UTIL_CONFIG_COMPILER_CXX_DECLTYPE_AUTO 0
#    endif

#    if _MSC_VER >= 1800
#      define UTIL_CONFIG_COMPILER_CXX_DEFAULT_FUNCTION_TEMPLATE_ARGS 1
#    else
#      define UTIL_CONFIG_COMPILER_CXX_DEFAULT_FUNCTION_TEMPLATE_ARGS 0
#    endif

#    if _MSC_VER >= 1800
#      define UTIL_CONFIG_COMPILER_CXX_DEFAULTED_FUNCTIONS 1
#    else
#      define UTIL_CONFIG_COMPILER_CXX_DEFAULTED_FUNCTIONS 0
#    endif

#    if _MSC_VER >= 1800
#      define UTIL_CONFIG_COMPILER_CXX_DELEGATING_CONSTRUCTORS 1
#    else
#      define UTIL_CONFIG_COMPILER_CXX_DELEGATING_CONSTRUCTORS 0
#    endif

#    if _MSC_VER >= 1900
#      define UTIL_CONFIG_COMPILER_CXX_DELETED_FUNCTIONS 1
#    else
#      define UTIL_CONFIG_COMPILER_CXX_DELETED_FUNCTIONS 0
#    endif

#    if _MSC_VER >= 1700
#      define UTIL_CONFIG_COMPILER_CXX_FINAL 1
#    else
#      define UTIL_CONFIG_COMPILER_CXX_FINAL 0
#    endif

#    if _MSC_VER >= 1900
#      define UTIL_CONFIG_COMPILER_CXX_GENERIC_LAMBDAS 1
#    else
#      define UTIL_CONFIG_COMPILER_CXX_GENERIC_LAMBDAS 0
#    endif

#    if _MSC_VER >= 1900
#      define UTIL_CONFIG_COMPILER_CXX_INHERITING_CONSTRUCTORS 1
#    else
#      define UTIL_CONFIG_COMPILER_CXX_INHERITING_CONSTRUCTORS 0
#    endif

#    if _MSC_VER >= 1600
#      define UTIL_CONFIG_COMPILER_CXX_LAMBDAS 1
#    else
#      define UTIL_CONFIG_COMPILER_CXX_LAMBDAS 0
#    endif

#    if _MSC_VER >= 1600
#      define UTIL_CONFIG_COMPILER_CXX_LONG_LONG_TYPE 1
#    else
#      define UTIL_CONFIG_COMPILER_CXX_LONG_LONG_TYPE 0
#    endif

#    if _MSC_VER >= 1900
#      define UTIL_CONFIG_COMPILER_CXX_NOEXCEPT 1
#    else
#      define UTIL_CONFIG_COMPILER_CXX_NOEXCEPT 0
#    endif

#    if _MSC_VER >= 1900
#      define UTIL_CONFIG_COMPILER_CXX_NONSTATIC_MEMBER_INIT 1
#    else
#      define UTIL_CONFIG_COMPILER_CXX_NONSTATIC_MEMBER_INIT 0
#    endif

#    if _MSC_VER >= 1600
#      define UTIL_CONFIG_COMPILER_CXX_NULLPTR 1
#    else
#      define UTIL_CONFIG_COMPILER_CXX_NULLPTR 0
#    endif

#    if _MSC_VER >= 1600
#      define UTIL_CONFIG_COMPILER_CXX_OVERRIDE 1
#    else
#      define UTIL_CONFIG_COMPILER_CXX_OVERRIDE 0
#    endif

#    if _MSC_VER >= 1700
#      define UTIL_CONFIG_COMPILER_CXX_RANGE_FOR 1
#    else
#      define UTIL_CONFIG_COMPILER_CXX_RANGE_FOR 0
#    endif

#    if _MSC_VER >= 1800
#      define UTIL_CONFIG_COMPILER_CXX_RAW_STRING_LITERALS 1
#    else
#      define UTIL_CONFIG_COMPILER_CXX_RAW_STRING_LITERALS 0
#    endif

#    if _MSC_VER >= 1911
#      define UTIL_CONFIG_COMPILER_CXX_RELAXED_CONSTEXPR 1
#    else
#      define UTIL_CONFIG_COMPILER_CXX_RELAXED_CONSTEXPR 0
#    endif

#    if _MSC_VER >= 1900
#      define UTIL_CONFIG_COMPILER_CXX_RETURN_TYPE_DEDUCTION 1
#    else
#      define UTIL_CONFIG_COMPILER_CXX_RETURN_TYPE_DEDUCTION 0
#    endif

#    if _MSC_VER >= 1600
#      define UTIL_CONFIG_COMPILER_CXX_RVALUE_REFERENCES 1
#    else
#      define UTIL_CONFIG_COMPILER_CXX_RVALUE_REFERENCES 0
#    endif

#    if _MSC_VER >= 1900
#      define UTIL_CONFIG_COMPILER_CXX_SIZEOF_MEMBER 1
#    else
#      define UTIL_CONFIG_COMPILER_CXX_SIZEOF_MEMBER 0
#    endif

#    if _MSC_VER >= 1600
#      define UTIL_CONFIG_COMPILER_CXX_STATIC_ASSERT 1
#    else
#      define UTIL_CONFIG_COMPILER_CXX_STATIC_ASSERT 0
#    endif

#    if _MSC_VER >= 1900
#      define UTIL_CONFIG_COMPILER_CXX_THREAD_LOCAL 1
#    else
#      define UTIL_CONFIG_COMPILER_CXX_THREAD_LOCAL 0
#    endif

#    if _MSC_VER >= 1800
#      define UTIL_CONFIG_COMPILER_CXX_VARIADIC_TEMPLATES 1
#    else
#      define UTIL_CONFIG_COMPILER_CXX_VARIADIC_TEMPLATES 0
#    endif

#  else
#    error Unsupported compiler
#  endif

#  ifndef UTIL_CONFIG_DEPRECATED
#    if defined(UTIL_CONFIG_COMPILER_CXX_ATTRIBUTE_DEPRECATED) && UTIL_CONFIG_COMPILER_CXX_ATTRIBUTE_DEPRECATED
#      define UTIL_CONFIG_DEPRECATED [[deprecated]]
#      define UTIL_CONFIG_DEPRECATED_MSG(MSG) [[deprecated(MSG)]]
#    elif UTIL_CONFIG_COMPILER_IS_GNU || UTIL_CONFIG_COMPILER_IS_Clang
#      define UTIL_CONFIG_DEPRECATED __attribute__((__deprecated__))
#      define UTIL_CONFIG_DEPRECATED_MSG(MSG) __attribute__((__deprecated__(MSG)))
#    elif UTIL_CONFIG_COMPILER_IS_MSVC
#      define UTIL_CONFIG_DEPRECATED __declspec(deprecated)
#      define UTIL_CONFIG_DEPRECATED_MSG(MSG) __declspec(deprecated(MSG))
#    else
#      define UTIL_CONFIG_DEPRECATED
#      define UTIL_CONFIG_DEPRECATED_MSG(MSG)
#    endif
#  endif

#  if defined(UTIL_CONFIG_COMPILER_CXX_CONSTEXPR) && UTIL_CONFIG_COMPILER_CXX_CONSTEXPR
#    define UTIL_CONFIG_CONSTEXPR constexpr
#  else
#    define UTIL_CONFIG_CONSTEXPR
#  endif

#  if defined(UTIL_CONFIG_COMPILER_CXX_DELETED_FUNCTIONS) && UTIL_CONFIG_COMPILER_CXX_DELETED_FUNCTIONS
#    define UTIL_CONFIG_DELETED_FUNCTION = delete
#  else
#    define UTIL_CONFIG_DELETED_FUNCTION
#  endif

#  if defined(UTIL_CONFIG_COMPILER_CXX_FINAL) && UTIL_CONFIG_COMPILER_CXX_FINAL
#    define UTIL_CONFIG_FINAL final
#  else
#    define UTIL_CONFIG_FINAL
#  endif

#  if defined(UTIL_CONFIG_COMPILER_CXX_NOEXCEPT) && UTIL_CONFIG_COMPILER_CXX_NOEXCEPT
#    define UTIL_CONFIG_NOEXCEPT noexcept
#    define UTIL_CONFIG_NOEXCEPT_EXPR(X) noexcept(X)
#  else
#    define UTIL_CONFIG_NOEXCEPT
#    define UTIL_CONFIG_NOEXCEPT_EXPR(X)
#  endif

#  if defined(UTIL_CONFIG_COMPILER_CXX_NULLPTR) && UTIL_CONFIG_COMPILER_CXX_NULLPTR
#    define UTIL_CONFIG_NULLPTR nullptr
#  elif UTIL_CONFIG_COMPILER_IS_GNU
#    define UTIL_CONFIG_NULLPTR __null
#  else
#    define UTIL_CONFIG_NULLPTR 0
#  endif

#  if defined(UTIL_CONFIG_COMPILER_CXX_OVERRIDE) && UTIL_CONFIG_COMPILER_CXX_OVERRIDE
#    define UTIL_CONFIG_OVERRIDE override
#  else
#    define UTIL_CONFIG_OVERRIDE
#  endif

#  if defined(UTIL_CONFIG_COMPILER_CXX_STATIC_ASSERT) && UTIL_CONFIG_COMPILER_CXX_STATIC_ASSERT
#    define UTIL_CONFIG_STATIC_ASSERT(X) static_assert(X, #    X)
#    define UTIL_CONFIG_STATIC_ASSERT_MSG(X, MSG) static_assert(X, MSG)
#  else
#    define UTIL_CONFIG_STATIC_ASSERT_JOIN(X, Y) UTIL_CONFIG_STATIC_ASSERT_JOIN_IMPL(X, Y)
#    define UTIL_CONFIG_STATIC_ASSERT_JOIN_IMPL(X, Y) X##Y
template <bool>
struct UTIL_CONFIGStaticAssert;
template <>
struct UTIL_CONFIGStaticAssert<true> {};
#    define UTIL_CONFIG_STATIC_ASSERT(X)                                                                           \
      enum {                                                                                                       \
        UTIL_CONFIG_STATIC_ASSERT_JOIN(UTIL_CONFIGStaticAssertEnum, __LINE__) = sizeof(UTIL_CONFIGStaticAssert<X>) \
      }
#    define UTIL_CONFIG_STATIC_ASSERT_MSG(X, MSG)                                                                  \
      enum {                                                                                                       \
        UTIL_CONFIG_STATIC_ASSERT_JOIN(UTIL_CONFIGStaticAssertEnum, __LINE__) = sizeof(UTIL_CONFIGStaticAssert<X>) \
      }
#  endif

#  if defined(UTIL_CONFIG_COMPILER_CXX_THREAD_LOCAL) && UTIL_CONFIG_COMPILER_CXX_THREAD_LOCAL
#    define UTIL_CONFIG_THREAD_LOCAL thread_local
#  elif UTIL_CONFIG_COMPILER_IS_GNU || UTIL_CONFIG_COMPILER_IS_Clang || UTIL_CONFIG_COMPILER_IS_AppleClang
#    define UTIL_CONFIG_THREAD_LOCAL __thread
#  elif UTIL_CONFIG_COMPILER_IS_MSVC
#    define UTIL_CONFIG_THREAD_LOCAL __declspec(thread)
#  else
// UTIL_CONFIG_THREAD_LOCAL not defined for this configuration.
#  endif

#endif

#endif
