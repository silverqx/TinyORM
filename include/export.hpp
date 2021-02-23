#ifndef TINYORM_EXPORT_H
#define TINYORM_EXPORT_H

#if defined(_MSC_VER) || defined(WIN64) || defined(_WIN64) || defined(__WIN64__) \
                      || defined(WIN32) || defined(_WIN32) || defined(__WIN32__) \
                      || defined(__NT__)
#  define TINY_DECL_EXPORT __declspec(dllexport)
#  define TINY_DECL_IMPORT __declspec(dllimport)
#else
#  define TINY_DECL_EXPORT __attribute__((visibility("default")))
#  define TINY_DECL_IMPORT __attribute__((visibility("default")))
#endif

#if defined(TINYORM_BUILDING_SHARED)
#  define SHAREDLIB_EXPORT TINY_DECL_EXPORT
#else
#  define SHAREDLIB_EXPORT TINY_DECL_IMPORT
#endif

#endif // TINYORM_EXPORT_H
