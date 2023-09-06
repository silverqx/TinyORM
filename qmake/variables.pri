# Variables to target a correct build folder
# ---

# Underscored Qt version (eg. 6_5_2)
TINY_QT_VERSION_UNDERSCORED = $$replace(QT_VERSION, \., _)

# Build type string (Debug, Profile, or Release)
CONFIG(debug, debug|release): \
    TINY_RELEASE_TYPE_CAMEL = Debug

else:CONFIG(release, debug|release): \
    force_debug_info: \
        TINY_RELEASE_TYPE_CAMEL = Profile
    else: \
        TINY_RELEASE_TYPE_CAMEL = Release

# Msvc compiler string (MSVC2022 or MSVC2019)
greaterThan(QMAKE_MSC_VER, 1929): \
    TINY_MSVC_VERSION = MSVC2022
else: \
    TINY_MSVC_VERSION = MSVC2019

win32-clang-msvc: \
    TINY_MSVC_VERSION = $${TINY_MSVC_VERSION}_LLVM

# Folder name by release type if CONFIG+=debug_and_release is defined (/debug, /release,
# or an empty string)
debug_and_release {
    CONFIG(release, debug|release): \
        TINY_BUILD_SUBFOLDER = $$quote(/release)

    else:CONFIG(debug, debug|release): \
        TINY_BUILD_SUBFOLDER = $$quote(/debug)
}
else: TINY_BUILD_SUBFOLDER =
