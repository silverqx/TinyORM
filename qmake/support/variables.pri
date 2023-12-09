# Variables and function to target the correct build folder
# ---
# These variables can be used to link against the TinyORM library in the build tree

# Get a directory that contains all qmake builds (build trees)
defineReplace(tinyorm_builds_dir) {
    # Nothing to do
    isEmpty(TINY_MAIN_DIR): return()

    return($$quote($$TINY_MAIN_DIR/TinyORM-builds-qmake))
}

# Guess and return a TinyORM build folder name from the CURRENT build folder name
defineReplace(tinyorm_build_folder) {
    # Nothing to do
    isEmpty(TINY_BUILD_TREE)|isEmpty(TARGET): return()

    currentBuildFolder = $$section(TINY_BUILD_TREE, /, -1)

    tinyormBuildFolder = $$replace(currentBuildFolder, $$TARGET, TinyORM)

    return($$tinyormBuildFolder)
}

# Get an absolute path to the TinyORM build tree
defineReplace(tinyorm_build_tree) {
    # Nothing to do
    isEmpty(TINYORM_BUILDS_DIR)|isEmpty(TINYORM_BUILD_FOLDER): return()

    return($$quote($$TINYORM_BUILDS_DIR/$$TINYORM_BUILD_FOLDER))
}

# Directory that contains all qmake builds (build trees)
isEmpty(TINYORM_BUILDS_DIR):   TINYORM_BUILDS_DIR   = $$tinyorm_builds_dir()
# TinyORM build folder name (guessed from the CURRENT build folder name)
isEmpty(TINYORM_BUILD_FOLDER): TINYORM_BUILD_FOLDER = $$tinyorm_build_folder()
# Absolute path to the TinyORM build tree
isEmpty(TINYORM_BUILD_TREE):   TINYORM_BUILD_TREE   = $$tinyorm_build_tree()

# To put the TinyORM build folder name manually
# ---
# All variables below can also be used to put together a path to the TinyORM build tree
# They were used in previous versions a better option is to use the variables above

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

# To correctly link ccache build against a ccache build (_ccache or an empty string)
contains(OUT_PWD, .*ccache.*): TINY_CCACHE_BUILD = _ccache
else: TINY_CCACHE_BUILD =
