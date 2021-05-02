#ifndef TINYORM_EXPORT_H
#define TINYORM_EXPORT_H

#if defined(_MSC_VER) || defined(WIN64) || defined(_WIN64) || defined(__WIN64__) \
                      || defined(WIN32) || defined(_WIN32) || defined(__WIN32__) \
                      || defined(__NT__)
#  define TINY_DECL_EXPORT __declspec(dllexport)
#  define TINY_DECL_IMPORT __declspec(dllimport)
#elif __GNU__ >= 4
#  define TINY_DECL_EXPORT __attribute__((visibility("default")))
#  define TINY_DECL_IMPORT __attribute__((visibility("default")))
#else
#  define TINY_DECL_EXPORT
#  define TINY_DECL_IMPORT
#endif

#if defined(TINYORM_BUILDING_SHARED)
#  define SHAREDLIB_EXPORT TINY_DECL_EXPORT
#elif defined(TINYORM_LINKING_SHARED)
#  define SHAREDLIB_EXPORT TINY_DECL_IMPORT
#endif

// Building static library
#ifndef SHAREDLIB_EXPORT
#  define SHAREDLIB_EXPORT
#endif

#endif // TINYORM_EXPORT_H
